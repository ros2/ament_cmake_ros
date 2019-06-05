# Copyright 2019 Apex.AI, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Add a pytest that runs with ROS_DOMAIN_ID isolation.
#
# Parameters are the same as ament_add_pytest_test

function(ament_add_pytest_isolated_test testname path)

  set(RUNNER "${domain_coordinator_DIR}/run_test_isolated.py")

  ament_add_pytest_test(
    "${testname}" "${path}"
    RUNNER "${RUNNER}"
    ${ARGN}
  )

endfunction()
