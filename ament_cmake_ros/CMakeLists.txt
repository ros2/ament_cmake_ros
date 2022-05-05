cmake_minimum_required(VERSION 3.5)

project(ament_cmake_ros NONE)

find_package(ament_cmake REQUIRED)

ament_export_dependencies(ament_cmake)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package(
  CONFIG_EXTRAS "ament_cmake_ros-extras.cmake.in"
)

if(WIN32)
  ament_environment_hooks(env_hook/warn_if_not_administrator.bat)
endif()

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
