cmake_minimum_required(VERSION 3.20)

project(ament_cmake_ros_core NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)
find_package(ament_cmake_export_targets REQUIRED)

ament_export_dependencies(ament_cmake_core)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

add_library(ament_ros_cxx_standard INTERFACE)
target_compile_features(ament_ros_cxx_standard INTERFACE cxx_std_20)
set_property(TARGET ament_ros_cxx_standard PROPERTY
  CXX_STANDARD_REQUIRED YES
)

add_library(ament_ros_c_standard INTERFACE)
target_compile_features(ament_ros_c_standard INTERFACE c_std_11)
set_property(TARGET ament_ros_c_standard PROPERTY
  C_STANDARD_REQUIRED YES
)

# Leaving blank for now but, adding here so if we wanted to unify in the future it would be easier
add_library(ament_ros_warnings INTERFACE)
target_compile_options(ament_ros_warnings INTERFACE)


add_library(ament_ros_defaults INTERFACE)

# TODO: Should BUILD_SHARED_LIBS and DROS_PACKAGE_NAME get set by this?
# Or is forcing downstream users to update to disruptive at this time.
# Maybe have users adopt ament_ros_defaults for 2-4 releases then move inside?
target_link_libraries(ament_ros_defaults INTERFACE
  ament_ros_cxx_standard
  ament_ros_c_standard
  ament_ros_warnings
)

install(
  TARGETS
    ament_ros_defaults
    ament_ros_cxx_standard
    ament_ros_c_standard
    ament_ros_warnings
  EXPORT ament_ros_core_targets
)

ament_export_targets(
  ament_ros_core_targets
)

ament_package(
  CONFIG_EXTRAS "ament_cmake_ros_core-extras.cmake.in"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
