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

#include <errno.h>
#include <stdio.h>

#if !defined _WIN32 && !defined __CYGWIN__
#include <signal.h>
#endif

#include <rcutils/allocator.h>
#include <rcutils/process.h>
#include <rcutils/strdup.h>
#include <rcutils/types.h>

#include <rmw_test_fixture/rmw_test_fixture.h>

#if defined _WIN32 || defined __CYGWIN__
bool ctrl_c_handler(int signal)
{
  return false;
}
#endif

int
main(int argc, char *argv[])
{
  int exit_code;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_ret_t rcutils_ret;
  rcutils_string_array_t args = rcutils_get_zero_initialized_string_array();
  rcutils_process_t * process;
  rmw_ret_t rmw_ret;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s PROGRAM_NAME [PROGRAM_ARGS ...]\n", argv[0]);
    return EINVAL;
  }

  rcutils_ret = rcutils_string_array_init(&args, argc - 1, &allocator);
  if (RCUTILS_RET_OK != rcutils_ret) {
    fprintf(stderr, "Failed to initialize argument array (%d)\n", rcutils_ret);
    return rcutils_ret;
  }

  for (int i = 1; i < argc; i++) {
    args.data[i - 1] = rcutils_strdup(argv[i], allocator);
    if (NULL == args.data[i - 1]) {
      fprintf(stderr, "Failed to populate argument array\n");
      return 1;
    }
  }

  rmw_ret = rmw_test_isolation_start();
  if (RMW_RET_OK != rmw_ret) {
    fprintf(stderr, "Failed to start RMW isolation (%d)\n", rmw_ret);
    return rmw_ret;
  }

  process = rcutils_start_process(&args, &allocator);
  if (NULL == process) {
    fprintf(stderr, "Failed to run %s\n", argv[1]);
    return 1;
  }

  rcutils_ret = rcutils_string_array_fini(&args);
  if (RCUTILS_RET_OK != rcutils_ret) {
    fprintf(stderr, "Failed to free argument array (%d)\n", rcutils_ret);
    return rcutils_ret;
  }

  // Let the signal propagate to the child process - this process should only
  // exit once the child process does.
#if defined _WIN32 || defined __CYGWIN__
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_c_handler, TRUE);
#else
  signal(SIGINT, SIG_IGN);
#endif

  rcutils_ret = rcutils_process_wait(process, &exit_code);
  if (RCUTILS_RET_OK != rcutils_ret) {
    fprintf(stderr, "Failed to wait for subprocess termination (%d)\n", rcutils_ret);
    return rcutils_ret;
  }

  rcutils_process_close(process);

  rmw_ret = rmw_test_isolation_stop();
  if (RMW_RET_OK != rmw_ret) {
    fprintf(stderr, "Failed to stop RMW isolation (%d)\n", rmw_ret);
    return rmw_ret;
  }

  return exit_code;
}
