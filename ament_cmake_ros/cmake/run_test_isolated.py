#!/usr/bin/env python3

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

import os
import sys

import ament_cmake_test
from ament_index_python import has_resource


def _get_rmw_isolator_path():
    prefix = has_resource('packages', 'rmw_test_fixture_implementation')
    if not prefix:
        raise RuntimeError(
            "Could not find package 'rmw_test_fixture_implementation'.")
    return os.path.join(
        prefix, 'lib', 'rmw_test_fixture_implementation', 'run_rmw_isolated')


if __name__ == '__main__':
    argv = sys.argv[1:]
    try:
        index = argv.index('--command')
    except ValueError:
        # Let the parser raise an error
        pass
    else:
        # Insert the isolation wrapper at the front of the command list
        argv.insert(index + 1, _get_rmw_isolator_path())

    sys.exit(ament_cmake_test.main(argv=argv))
