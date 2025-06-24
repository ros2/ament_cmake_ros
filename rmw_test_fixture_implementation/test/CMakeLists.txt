find_package(ament_cmake_gtest REQUIRED)
find_package(ament_cmake_pytest REQUIRED)

add_library(rmw_mockmock_test_fixture SHARED
  rmw_mockmock_test_fixture.c
)

target_compile_definitions(rmw_mockmock_test_fixture PRIVATE
  RMW_TEST_FIXTURE_BUILDING_DLL
)

target_link_libraries(rmw_mockmock_test_fixture
  PRIVATE
    rcutils::rcutils
    rmw_test_fixture::rmw_test_fixture
)

ament_add_gtest(test_fixture_start_stop
  test_fixture_start_stop.cpp
  ENV
    RMW_IMPLEMENTATION=rmw_mockmock
  APPEND_LIBRARY_DIRS
    $<TARGET_FILE_DIR:rmw_mockmock_test_fixture>
)

target_link_libraries(test_fixture_start_stop
  rcpputils::rcpputils
  rmw_test_fixture::rmw_test_fixture
  rmw_test_fixture_implementation
)

ament_add_pytest_test(test_fixture_start_stop_py
  test_fixture_start_stop.py
  ENV
    RMW_IMPLEMENTATION=rmw_mockmock
  APPEND_ENV
    PYTHONPATH=$<TARGET_FILE_DIR:_rmw_test_fixture_implementation>
  APPEND_LIBRARY_DIRS
    $<TARGET_FILE_DIR:rmw_mockmock_test_fixture>
)
