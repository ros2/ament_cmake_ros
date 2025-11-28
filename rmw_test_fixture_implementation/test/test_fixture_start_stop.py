# Copyright 2025 Open Source Robotics Foundation, Inc.
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

import os

from rpyutils import add_dll_directories_from_env

with add_dll_directories_from_env('PATH'):
    from _rmw_test_fixture_implementation import rmw_test_isolation_start
    from _rmw_test_fixture_implementation import rmw_test_isolation_stop


def test_start_stop():
    assert not os.environ.get('MOCK_FIXTURE_START_CALLED')
    assert not os.environ.get('MOCK_FIXTURE_STOP_CALLED')

    rmw_test_isolation_start()

    assert '1' == os.environ.get('MOCK_FIXTURE_START_CALLED')
    assert not os.environ.get('MOCK_FIXTURE_STOP_CALLED')

    rmw_test_isolation_stop()

    assert not os.environ.get('MOCK_FIXTURE_START_CALLED')
    assert '1' == os.environ.get('MOCK_FIXTURE_STOP_CALLED')
