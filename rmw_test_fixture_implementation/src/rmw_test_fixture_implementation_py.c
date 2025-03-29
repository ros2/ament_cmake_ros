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

#include <rmw_test_fixture/rmw_test_fixture.h>

static PyObject * get_fresh_environ()
{
  PyThreadState *sub_state = Py_NewInterpreter();
  if (NULL == sub_state) {
    return NULL;
  }

  PyObject *os = PyImport_ImportModule("os");
  if (NULL == os) {
    Py_EndInterpreter(sub_state);
    return NULL;
  }

  PyObject *os_environ = PyObject_GetAttrString(os, "environ");
  Py_DECREF(os);
  if (NULL == os_environ) {
    return NULL;
  }

  PyObject *fresh_environ = PyMapping_Items(os_environ);
  Py_DECREF(os_environ);
  Py_EndInterpreter(sub_state);
  return fresh_environ;
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
static bool reload_environ()
{
  PyThreadState *orig_state = PyThreadState_Swap(NULL);

  PyObject * fresh_environ = get_fresh_environ();
  PyThreadState_Swap(orig_state);
  if (NULL == fresh_environ) {
    return false;
  }

  PyObject *os = PyImport_ImportModule("os");
  if (NULL == os) {
    Py_DECREF(fresh_environ);
    return false;
  }

  PyObject *os_environ = PyObject_GetAttrString(os, "environ");
  Py_DECREF(os);
  if (NULL == os_environ) {
    Py_DECREF(fresh_environ);
    return false;
  }

  PyObject *res = PyObject_CallMethod(os_environ, "clear", NULL);
  if (NULL == res) {
    Py_DECREF(os_environ);
    Py_DECREF(fresh_environ);
    return false;
  }

  res = PyObject_CallMethod(os_environ, "update", "O", fresh_environ);
  Py_DECREF(os_environ);
  Py_DECREF(fresh_environ);

  return true;
}

static PyObject *
isolation_start(PyObject * Py_UNUSED(self), PyObject * Py_UNUSED(args))
{
  rmw_ret_t ret = rmw_test_isolation_start();
  if (RMW_RET_OK != ret) {
    PyErr_Format(PyExc_RuntimeError, "Failed to start RMW isolation (%d)", ret);
    return NULL;
  }

  if (!reload_environ()) {
    PyErr_Format(PyExc_RuntimeError, "Failed to reload environment");
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

  if (!reload_environ()) {
    PyErr_Format(PyExc_RuntimeError, "Failed to reload environment");
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
