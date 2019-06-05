cmake_minimum_required(VERSION 3.5)

project(ament_cmake_test_isolated)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
