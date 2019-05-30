cmake_minimum_required(VERSION 3.5)

project(ament_cmake_pytest_isolated)

find_package(ament_cmake REQUIRED)
find_package(domain_coordinator REQUIRED)

ament_export_dependencies(domain_coordinator)

ament_package(
  CONFIG_EXTRAS "ament_cmake_pytest_isolated-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
