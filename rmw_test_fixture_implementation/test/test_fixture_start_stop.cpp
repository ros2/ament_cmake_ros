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

#include <gtest/gtest.h>
#include <rmw_test_fixture/rmw_test_fixture.h>

#include <rcpputils/env.hpp>

TEST(RMWTestFixture, start_stop) {
  ASSERT_EQ(
    "",
    rcpputils::get_env_var("MOCK_FIXTURE_START_CALLED"));
  ASSERT_EQ(
    "",
    rcpputils::get_env_var("MOCK_FIXTURE_STOP_CALLED"));

  EXPECT_EQ(
    RCUTILS_RET_OK,
    rmw_test_isolation_start());

  EXPECT_EQ(
    "1",
    rcpputils::get_env_var("MOCK_FIXTURE_START_CALLED"));
  EXPECT_EQ(
    "",
    rcpputils::get_env_var("MOCK_FIXTURE_STOP_CALLED"));

  EXPECT_EQ(
    RCUTILS_RET_OK,
    rmw_test_isolation_stop());

  EXPECT_EQ(
    "",
    rcpputils::get_env_var("MOCK_FIXTURE_START_CALLED"));
  EXPECT_EQ(
    "1",
    rcpputils::get_env_var("MOCK_FIXTURE_STOP_CALLED"));
}
