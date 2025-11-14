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

#if defined _WIN32 || defined __CYGWIN__
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
// When building with MSVC 19.28.29333.0 on Windows 10 (as of 2020-11-11),
// there appears to be a problem with winbase.h (which is included by
// Windows.h). In particular, warnings of the form:
//
// warning C5105: macro expansion producing 'defined' has undefined behavior
//
// See https://developercommunity.visualstudio.com/content/problem/695656/wdk-and-sdk-are-not-compatible-with-experimentalpr.html
// for more information. For now disable that warning when including windows.h
#pragma warning(push)
#pragma warning(disable : 5105)
#include <Windows.h>
#include <winsock2.h>
#pragma warning(pop)
#else
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <errno.h>
#include <rcutils/env.h>
#include <rcutils/format_string.h>
#include <rcutils/process.h>
#include <rcutils/strdup.h>
#include <rmw/types.h>

#if defined _WIN32 || defined __CYGWIN__
typedef SOCKET port_lock_t;
#define INVALID_PORT_LOCK INVALID_SOCKET
#else
typedef int port_lock_t;
#define INVALID_PORT_LOCK -1
#endif

static port_lock_t g_lock = INVALID_PORT_LOCK;

static unsigned int g_random_seed;

static bool g_random_seed_initialized = false;

static char *g_restore_domain_id = NULL;

static
void
port_lock_fini(port_lock_t lock)
{
  if (INVALID_PORT_LOCK == lock) {
    return;
  }

#if defined _WIN32 || defined __CYGWIN__
  closesocket(lock);
  WSACleanup();
#else
  close(lock);
#endif
}

/// Acquire a global lock by binding to TCP ports on localhost
/**
 * This function implements a primitive locking mechanism by binding to TCP
 * port numbers in a specific range. It loops through all of the ports in the
 * range and returns when it successful binds to one of them. The index into
 * the range changes based on the process ID, but the entire range will
 * eventually be searched for a free slot.
 *
 * \param[in] start The first port number in the allocation range.
 * \param[in] end The last port number in the allocation range.
 * \param[out] slot Upon successful lock, this will be set to the index
 *   within the port range which was bound.
 * \return The allocated port lock, or
 * \return `INVALID_PORT_LOCK` if an unexpected error occurs.
 */
static
port_lock_t
port_lock_init(uint16_t start, uint16_t end, uint16_t *slot)
{
  if (end <= start) {
    return INVALID_PORT_LOCK;
  }

#if defined _WIN32 || defined __CYGWIN__
  WSADATA wsa_data = {0};
  if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
    return INVALID_PORT_LOCK;
  }
#endif

  port_lock_t lock = socket(AF_INET, SOCK_STREAM, 0);
  if (INVALID_PORT_LOCK == lock) {
#if defined _WIN32 || defined __CYGWIN__
    WSACleanup();
#endif
    return INVALID_PORT_LOCK;
  }

  if (!g_random_seed_initialized) {
    g_random_seed = rcutils_get_pid();
    g_random_seed_initialized = true;
  }

  g_random_seed = g_random_seed * 1103515245 + 12345;
  uint16_t offset = g_random_seed % (end - start);

  struct sockaddr_in addr;
  memset(&addr, 0x0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  for (uint16_t attempt = 0; start + attempt < end; attempt++) {
    uint16_t candidate = start + ((attempt + offset) % (end - start));
    addr.sin_port = htons(candidate);

    if (0 == bind(lock, (struct sockaddr *)&addr, sizeof(addr))) {
      if (NULL != slot) {
        *slot = candidate - start;
      }
      return lock;
    }
  }

  port_lock_fini(lock);

  return INVALID_PORT_LOCK;
}

/// Isolate ROS communication using a unique ROS_DOMAIN_ID
/**
 * In an attempt to prevent crosstalk between tests, this fixture sets a
 * unique ROS_DOMAIN_ID for the current process and also limits ROS
 * communication to localhost.
 *
 * The ROS_DOMAIN_ID is chosen by binding to a TCP port in a known range within
 * the ephemeral port range to "claim" the ID and prevent other tests using
 * this fixture from using it until the test is finished.
 *
 * This approach does not isolate the test from other processes on the host
 * which may choose the same ROS_DOMAIN_ID, but uses only standard ROS
 * configuration mechanisms to configure the testing environment. RMW
 * implementations are encouraged to implement more robust test fixtures
 * specific to their RMW provider.
 *
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs.
 */
rmw_ret_t
rmw_test_isolation_start_default(void)
{
  if (INVALID_PORT_LOCK != g_lock) {
    fprintf(stderr, "Fixture is already in use\n");
    return RMW_RET_ERROR;
  }

  uint16_t slot;
  g_lock = port_lock_init(32769, 32870, &slot);
  if (INVALID_PORT_LOCK == g_lock) {
    fprintf(stderr, "Failed to acquire port lock\n");
    return RMW_RET_ERROR;
  }

  // Avoid ROS_DOMAIN_ID=0 entirely
  slot += 1;

  const char *old_env_val;
  if (NULL != rcutils_get_env("ROS_DOMAIN_ID", &old_env_val)) {
    fprintf(stderr, "Failed to get ROS_DOMAIN_ID\n");
    port_lock_fini(g_lock);
    g_lock = INVALID_PORT_LOCK;
    return RMW_RET_ERROR;
  }

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (NULL != g_restore_domain_id) {
    allocator.deallocate(g_restore_domain_id, allocator.state);
    g_restore_domain_id = NULL;
  }
  if (strlen(old_env_val) > 0) {
    // rcutils_get_env yields an empty string if the variable is not set.
    // An empty ROS_DOMAIN_ID is not valid, so we should interpret an empty
    // value as "unset" and therefore pass NULL to rcutils_set_env on stop.
    g_restore_domain_id = rcutils_strdup(old_env_val, allocator);
    if (NULL == g_restore_domain_id) {
      fprintf(stderr, "Failed save old ROS_DOMAIN_ID\n");
      port_lock_fini(g_lock);
      g_lock = INVALID_PORT_LOCK;
      return RMW_RET_ERROR;
    }
  }

  char *env_val = rcutils_format_string(allocator, "%d", slot);
  if (NULL == env_val) {
    fprintf(stderr, "Failed to format ROS_DOMAIN_ID value\n");
    port_lock_fini(g_lock);
    g_lock = INVALID_PORT_LOCK;
    return RMW_RET_ERROR;
  }

  if (!rcutils_set_env("ROS_DOMAIN_ID", env_val)) {
    fprintf(stderr, "Failed to update ROS_DOMAIN_ID\n");
    allocator.deallocate(env_val, &allocator.state);
    port_lock_fini(g_lock);
    g_lock = INVALID_PORT_LOCK;
    return RMW_RET_ERROR;
  }

  allocator.deallocate(env_val, &allocator.state);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_test_isolation_stop_default(void)
{
  if (!rcutils_set_env("ROS_DOMAIN_ID", g_restore_domain_id)) {
    fprintf(stderr, "Failed to restore ROS_DOMAIN_ID\n");
  }

  if (NULL != g_restore_domain_id) {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    allocator.deallocate(g_restore_domain_id, allocator.state);
    g_restore_domain_id = NULL;
  }

  port_lock_fini(g_lock);
  g_lock = INVALID_PORT_LOCK;

  return RMW_RET_OK;
}
