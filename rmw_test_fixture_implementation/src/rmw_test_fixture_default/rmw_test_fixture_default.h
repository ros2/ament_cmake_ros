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

#ifndef RMW_TEST_FIXTURE_DEFAULT__RMW_TEST_FIXTURE_DEFAULT_H_
#define RMW_TEST_FIXTURE_DEFAULT__RMW_TEST_FIXTURE_DEFAULT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rmw/types.h>

rmw_ret_t
rmw_test_isolation_start_default(void);

rmw_ret_t
rmw_test_isolation_stop_default(void);

#ifdef __cplusplus
}
#endif

#endif  // RMW_TEST_FIXTURE_DEFAULT__RMW_TEST_FIXTURE_DEFAULT_H_
