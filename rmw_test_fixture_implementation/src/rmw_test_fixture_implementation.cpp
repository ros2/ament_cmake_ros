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

#include "rmw_test_fixture_default/rmw_test_fixture_default.h"

#include <rmw/macros.h>
#include <rmw/rmw.h>
#include <rmw/types.h>

#include <rmw_test_fixture/rmw_test_fixture.h>

#include <memory>
#include <stdexcept>
#include <string>

#include <rcpputils/env.hpp>
#include <rcpputils/shared_library.hpp>

static
rmw_ret_t
rmw_test_isolation_init();

static
rmw_ret_t
rmw_test_isolation_stop_noop();

static rmw_ret_t (*symbol_rmw_test_isolation_start)() = rmw_test_isolation_init;
static rmw_ret_t (*symbol_rmw_test_isolation_stop)() = rmw_test_isolation_stop_noop;

static std::unique_ptr<rcpputils::SharedLibrary> g_isolation_lib = nullptr;

static
rmw_ret_t
rmw_test_isolation_start_noop()
{
  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_test_isolation_stop_noop()
{
  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_test_isolation_init()
{
  if (!rcpputils::get_env_var("RMW_TEST_FIXTURE_DISABLE_ISOLATION").empty()) {
    symbol_rmw_test_isolation_start = rmw_test_isolation_start_noop;
    return symbol_rmw_test_isolation_start();
  }

  const char * rmw_id = rmw_get_implementation_identifier();

  if (rmw_id != NULL) {
    std::string library = std::string(rmw_id) + "_test_fixture";
    std::string library_name = rcpputils::get_platform_library_name(library);

    try {
      g_isolation_lib = std::make_unique<rcpputils::SharedLibrary>(library_name);
    } catch (const std::runtime_error & /*e*/) {
      // no library available, fall back to default isolation
    }
  }

  if (g_isolation_lib) {
    void *symbol = g_isolation_lib->get_symbol("rmw_test_isolation_start");
    if (symbol == nullptr) {
      g_isolation_lib.reset();
      return RMW_RET_ERROR;
    }

    symbol_rmw_test_isolation_start = (rmw_ret_t (*)())symbol;

    symbol = g_isolation_lib->get_symbol("rmw_test_isolation_stop");
    if (symbol != nullptr) {
      symbol_rmw_test_isolation_stop = (rmw_ret_t (*)())symbol;
    }
  } else {
    symbol_rmw_test_isolation_start = rmw_test_isolation_start_default;
    symbol_rmw_test_isolation_stop = rmw_test_isolation_stop_default;
  }

  return symbol_rmw_test_isolation_start();
}

#ifdef __cplusplus
extern "C"
{
#endif

rmw_ret_t
rmw_test_isolation_start()
{
  return symbol_rmw_test_isolation_start();
}

rmw_ret_t
rmw_test_isolation_stop()
{
  return symbol_rmw_test_isolation_stop();
}

#ifdef __cplusplus
}
#endif
