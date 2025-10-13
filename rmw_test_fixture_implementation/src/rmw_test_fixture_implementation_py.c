// Copyright 2025 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>

#include <Python.h>

#include <rcutils/allocator.h>
#include <rcutils/strdup.h>
#include <rcutils/types.h>
#include <rmw/convert_rcutils_ret_to_rmw_ret.h>
#include <rmw_test_fixture/rmw_test_fixture.h>

static rcutils_ret_t add_py_string_to_list(PyObject *py_str, rcutils_array_list_t *list)
{
  Py_ssize_t str_size;
  const char *raw_str = PyUnicode_AsUTF8AndSize(py_str, &str_size);
  if (NULL == raw_str) {
    return RCUTILS_RET_ERROR;
  }

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_char_array_t char_array = rcutils_get_zero_initialized_char_array();
  rcutils_ret_t ret = rcutils_char_array_init(&char_array, str_size + 1, &allocator);
  if (ret) {
    return ret;
  }

  ret = rcutils_char_array_memcpy(&char_array, raw_str, str_size);
  if (ret) {
    if (rcutils_char_array_fini(&char_array)) {
      RCUTILS_SET_ERROR_MSG("failed to deallocate char_array_t");
    }
    return ret;
  }

  ret = rcutils_array_list_add(list, &char_array);
  if (ret) {
    if (rcutils_char_array_fini(&char_array)) {
      RCUTILS_SET_ERROR_MSG("failed to deallocate char_array_t");
    }
    return ret;
  }

  return RCUTILS_RET_OK;
}

static rcutils_ret_t py_mapping_to_pair_list(PyObject *mapping, rcutils_array_list_t *pairs)
{
  PyObject *items = PyMapping_Items(mapping);
  if (NULL == items) {
    return RCUTILS_RET_ERROR;
  }

  Py_ssize_t size = PyMapping_Size(items);
  if (-1 == size) {
    Py_DECREF(items);
    return RCUTILS_RET_ERROR;
  }

  rcutils_ret_t ret;

  for (Py_ssize_t i = 0; i < size; i++) {
    PyObject *pair = PySequence_Fast_GET_ITEM(items, i);
    if (NULL == pair) {
      Py_DECREF(items);
      return RCUTILS_RET_ERROR;
    }

    PyObject *key = PySequence_Fast_GET_ITEM(pair, 0);
    if (NULL == key) {
      Py_DECREF(items);
      return RCUTILS_RET_ERROR;
    }

    ret = add_py_string_to_list(key, pairs);
    if (ret) {
      Py_DECREF(items);
      return ret;
    }

    PyObject *val = PySequence_Fast_GET_ITEM(pair, 1);
    if (NULL == val) {
      Py_DECREF(items);
      return RCUTILS_RET_ERROR;
    }

    ret = add_py_string_to_list(val, pairs);
    if (ret) {
      Py_DECREF(items);
      return ret;
    }
  }

  Py_DECREF(items);

  return RCUTILS_RET_OK;
}

static PyObject * get_py_string_from_list(rcutils_array_list_t *list, size_t index)
{
  rcutils_char_array_t char_array;
  if (rcutils_array_list_get(list, index, &char_array)) {
    return NULL;
  }

  return PyUnicode_FromStringAndSize(char_array.buffer, char_array.buffer_length);
}

static rcutils_ret_t pair_list_to_py_mapping(rcutils_array_list_t *pairs, PyObject *mapping)
{
  size_t size;
  rcutils_ret_t ret = rcutils_array_list_get_size(pairs, &size);
  if (ret) {
    return ret;
  } else if (size % 2) {
    return RCUTILS_RET_ERROR;
  }

  PyObject *res = PyObject_CallMethod(mapping, "clear", NULL);
  if (NULL == res) {
    return RCUTILS_RET_ERROR;
  }
  Py_DECREF(res);

  for (size_t i = 0; i < size; i += 2) {
    PyObject *key = get_py_string_from_list(pairs, i);
    if (NULL == key) {
      return RCUTILS_RET_ERROR;
    }

    PyObject *val = get_py_string_from_list(pairs, i + 1);
    if (NULL == val) {
      Py_DECREF(key);
      return RCUTILS_RET_ERROR;
    }

    if (PyObject_SetItem(mapping, key, val)) {
      Py_DECREF(val);
      Py_DECREF(key);
      return RCUTILS_RET_ERROR;
    }

    Py_DECREF(val);
    Py_DECREF(key);
  }

  return RCUTILS_RET_OK;
}

static rcutils_ret_t get_fresh_environ(rcutils_array_list_t *pairs)
{
  PyThreadState *sub_state = Py_NewInterpreter();
  if (NULL == sub_state) {
    return RCUTILS_RET_ERROR;
  }

  PyObject *os = PyImport_ImportModule("os");
  if (NULL == os) {
    Py_EndInterpreter(sub_state);
    return RCUTILS_RET_ERROR;
  }

  PyObject *os_environ = PyObject_GetAttrString(os, "environ");
  Py_DECREF(os);
  if (NULL == os_environ) {
    Py_EndInterpreter(sub_state);
    return RCUTILS_RET_ERROR;
  }

  rcutils_ret_t ret = py_mapping_to_pair_list(os_environ, pairs);
  Py_DECREF(os_environ);
  Py_EndInterpreter(sub_state);
  return ret;
}

static rcutils_ret_t fini_array_and_contents(rcutils_array_list_t *array_list)
{
  size_t size;
  rcutils_ret_t ret = rcutils_array_list_get_size(array_list, &size);
  if (ret) {
    return ret;
  }

  for (; size > 0; size--) {
    rcutils_char_array_t char_array;
    ret = rcutils_array_list_get(array_list, size - 1, &char_array);
    if (ret < 0) {
      return ret;
    }

    ret = rcutils_array_list_remove(array_list, size - 1);
    if (ret < 0) {
      return ret;
    }

    ret = rcutils_char_array_fini(&char_array);
    if (ret < 0) {
      return ret;
    }
  }

  return rcutils_array_list_fini(array_list);
}

/// Reload Python's os.environ from the process values.
/**
 * Python maintains a cache of the environment variables and when they are
 * modified from C, Python doesn't have any way to know. It appears that
 * Python 3.14 will provide a supported mechanism to reload the variables,
 * but for now, this process appears to safely achieve the same effect.
 *
 * At a high level:
 * 1. Spin up a new sub-interpreter. Upon importing `os`, it will reload
 *    the environment variables from C.
 * 2. Copy the list of environment variables out of `os.environ`.
 * 3. Destroy the sub-interpreter and switch back to the original one.
 * 4. Clear all of the environment variables from `os.environ`.
 * 5. Re-populate the variables with the fresh ones dumped from the
 *    sub-interpreter.
 *
 * \return true if the variables were reloaded successfully, or
 * \return false if an unspecified error occurs.
 */
static rmw_ret_t reload_environ(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_array_list_t pairs = rcutils_get_zero_initialized_array_list();
  rcutils_ret_t ret = rcutils_array_list_init(&pairs, 16, sizeof(rcutils_char_array_t), &allocator);
  if (ret) {
    return rmw_convert_rcutils_ret_to_rmw_ret(ret);
  }

  PyThreadState *orig_state = PyThreadState_Swap(NULL);

  ret = get_fresh_environ(&pairs);
  PyThreadState_Swap(orig_state);
  if (ret) {
    fini_array_and_contents(&pairs);
    return rmw_convert_rcutils_ret_to_rmw_ret(ret);
  }

  PyObject *os = PyImport_ImportModule("os");
  if (NULL == os) {
    fini_array_and_contents(&pairs);
    return RMW_RET_ERROR;
  }

  PyObject *os_environ = PyObject_GetAttrString(os, "environ");
  Py_DECREF(os);
  if (NULL == os_environ) {
    fini_array_and_contents(&pairs);
    return RMW_RET_ERROR;
  }

  PyObject *res = PyObject_CallMethod(os_environ, "clear", NULL);
  if (NULL == res) {
    Py_DECREF(os_environ);
    fini_array_and_contents(&pairs);
    return RMW_RET_ERROR;
  }

  ret = pair_list_to_py_mapping(&pairs, os_environ);
  Py_DECREF(os_environ);
  fini_array_and_contents(&pairs);

  return rmw_convert_rcutils_ret_to_rmw_ret(ret);
}

static PyObject *
isolation_start(PyObject * Py_UNUSED(self), PyObject * Py_UNUSED(args))
{
  rmw_ret_t ret = rmw_test_isolation_start();
  if (RMW_RET_OK != ret) {
    PyErr_Format(PyExc_RuntimeError, "Failed to start RMW isolation (%d)", ret);
    return NULL;
  }

  ret = reload_environ();
  if (ret) {
    PyErr_Format(PyExc_RuntimeError, "Failed to reload environment (%u)", ret);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *
isolation_stop(PyObject * Py_UNUSED(self), PyObject * Py_UNUSED(args))
{
  rmw_ret_t ret = rmw_test_isolation_stop();
  if (RMW_RET_OK != ret) {
    PyErr_Format(PyExc_RuntimeError, "Failed to stop RMW isolation (%d)", ret);
    return NULL;
  }

  ret = reload_environ();
  if (ret) {
    PyErr_Format(PyExc_RuntimeError, "Failed to reload environment (%d)", ret);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
  {"rmw_test_isolation_start", isolation_start, METH_NOARGS, "Start RMW isolation."},
  {"rmw_test_isolation_stop", isolation_stop, METH_NOARGS, "Stop RMW isolation."},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
  PyModuleDef_HEAD_INIT,
  "_rmw_test_fixtupre_implementation",
  NULL,
  -1,
  module_methods,
  NULL,
  NULL,
  NULL,
  NULL,
};

PyMODINIT_FUNC
PyInit__rmw_test_fixture_implementation(void)
{
  return PyModule_Create(&module);
}
