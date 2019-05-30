cmake_minimum_required(VERSION 3.5)

project(domain_coordinator)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(domain_coordinator_test "test/")
endif()

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
