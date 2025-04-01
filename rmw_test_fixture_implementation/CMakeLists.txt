cmake_minimum_required(VERSION 3.5)
project(rmw_test_fixture_implementation LANGUAGES C CXX)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# By default, without the settings below, find_package(Python3) will attempt
# to find the newest python version it can, and additionally will find the
# most specific version.  For instance, on a system that has
# /usr/bin/python3.10, /usr/bin/python3.11, and /usr/bin/python3, it will find
# /usr/bin/python3.11, even if /usr/bin/python3 points to /usr/bin/python3.10.
# The behavior we want is to prefer the "system" installed version unless the
# user specifically tells us othewise through the Python3_EXECUTABLE hint.
# Setting CMP0094 to NEW means that the search will stop after the first
# python version is found.  Setting Python3_FIND_UNVERSIONED_NAMES means that
# the search will prefer /usr/bin/python3 over /usr/bin/python3.11.  And that
# latter functionality is only available in CMake 3.20 or later, so we need
# at least that version.
cmake_policy(SET CMP0094 NEW)
set(Python3_FIND_UNVERSIONED_NAMES FIRST)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros_core REQUIRED)

find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(rmw_implementation REQUIRED)
find_package(rmw_test_fixture REQUIRED)

add_library(rmw_test_fixture_implementation
  src/rmw_test_fixture_default/rmw_test_fixture_default.c
  src/rmw_test_fixture_implementation.cpp
)
target_include_directories(rmw_test_fixture_implementation PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_definitions(rmw_test_fixture_implementation PRIVATE
  RMW_TEST_FIXTURE_BUILDING_DLL
)
target_link_libraries(rmw_test_fixture_implementation PRIVATE
  rcpputils::rcpputils
  rcutils::rcutils
  rmw::rmw
  rmw_implementation::rmw_implementation
  $<$<BOOL:${WIN32}>:ws2_32 wsock32>
)
target_link_libraries(rmw_test_fixture_implementation PUBLIC
  rmw_test_fixture::rmw_test_fixture
)

install(
  TARGETS rmw_test_fixture_implementation
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

add_executable(run_rmw_isolated
  src/run_rmw_isolated.c
)
target_link_libraries(run_rmw_isolated
  rcutils::rcutils
  rmw_test_fixture_implementation
)

install(
  TARGETS run_rmw_isolated
  EXPORT ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

ament_python_install_package(${PROJECT_NAME})

python3_add_library(_rmw_test_fixture_implementation
  src/rmw_test_fixture_implementation_py.c
)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  # python3_add_library should really take care of this for us, but it doesn't
  set_target_properties(_rmw_test_fixture_implementation PROPERTIES DEBUG_POSTFIX "_d")
endif()

# Set output directories to import module from the build directory
# Use a no-op generator expression so multi-config generators don't append an
# extra directory like Release/ or Debug/ and break the Python import.
set_target_properties(_rmw_test_fixture_implementation PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_BINARY_DIR}/test_${PROJECT_NAME}>"
  RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_BINARY_DIR}/test_${PROJECT_NAME}>"
)

target_link_libraries(_rmw_test_fixture_implementation PRIVATE
  rmw_test_fixture_implementation
)

install(TARGETS
  _rmw_test_fixture_implementation
  DESTINATION ${PYTHON_INSTALL_DIR}/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_targets(${PROJECT_NAME})
ament_export_dependencies(
  rmw_test_fixture
)

ament_package()
