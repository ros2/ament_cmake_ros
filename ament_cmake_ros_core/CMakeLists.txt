cmake_minimum_required(VERSION 3.20)

project(ament_cmake_ros_core NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)

ament_export_dependencies(ament_cmake_core)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package(
  CONFIG_EXTRAS "ament_cmake_ros_core-extras.cmake.in"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
