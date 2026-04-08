# Copyright 2026 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_library(ament_ros_cxx_standard INTERFACE)
target_compile_features(ament_ros_cxx_standard INTERFACE cxx_std_20)

add_library(ament_ros_c_standard INTERFACE)
target_compile_features(ament_ros_c_standard INTERFACE c_std_17)

add_library(ament_ros_defaults INTERFACE)
target_link_libraries(ament_ros_defaults INTERFACE
  ament_ros_cxx_standard
  ament_ros_c_standard
)
