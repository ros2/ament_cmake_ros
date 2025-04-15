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

from contextlib import AbstractContextManager

from rpyutils import add_dll_directories_from_env

# Since Python 3.8, on Windows we should ensure DLL directories are explicitly
# added to the search path.
# See https://docs.python.org/3/whatsnew/3.8.html#bpo-36085-whatsnew
with add_dll_directories_from_env('PATH'):
    from rmw_test_fixture_implementation._rmw_test_fixture_implementation \
        import rmw_test_isolation_start
    from rmw_test_fixture_implementation._rmw_test_fixture_implementation \
        import rmw_test_isolation_stop

__all__ = ['RMWTestIsolator']


class RMWTestIsolator(AbstractContextManager):
    """Manages RMW test isolation for the current process."""

    def __enter__(self):
        rmw_test_isolation_start()
        return super().__enter__()

    def __exit__(self, exc_type, exc_value, traceback):
        rmw_test_isolation_stop()
        return super().__exit__(exc_type, exc_value, traceback)
