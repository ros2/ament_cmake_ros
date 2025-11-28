^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rmw_test_fixture_implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.14.6 (2025-11-28)
-------------------
* Restore ROS_DOMAIN_ID after isolation is finished (`#58 <https://github.com/ros2/ament_cmake_ros/issues/58>`_) (`#59 <https://github.com/ros2/ament_cmake_ros/issues/59>`_)
* Contributors: mergify[bot]

0.14.5 (2025-10-31)
-------------------
* default to c++17 due to use of newer methods on std::map (`#57 <https://github.com/ros2/ament_cmake_ros/issues/57>`_)
* Contributors: William Woodall

0.14.4 (2025-10-17)
-------------------
* Copy all environment variables explicitly (backport `#43 <https://github.com/ros2/ament_cmake_ros/issues/43>`_) (`#52 <https://github.com/ros2/ament_cmake_ros/issues/52>`_)
* fix cmake deprecation (`#47 <https://github.com/ros2/ament_cmake_ros/issues/47>`_) (`#48 <https://github.com/ros2/ament_cmake_ros/issues/48>`_)
* Split the generator expression for each library (`#36 <https://github.com/ros2/ament_cmake_ros/issues/36>`_) (`#38 <https://github.com/ros2/ament_cmake_ros/issues/38>`_)
* Removed clang warnings (`#34 <https://github.com/ros2/ament_cmake_ros/issues/34>`_)
* Contributors: Alejandro Hernández Cordero, mergify[bot]

0.14.3 (2025-04-18)
-------------------

0.14.2 (2025-04-18)
-------------------
* Don't set ROS_AUTOMATIC_DISCOVERY_RANGE in rmw_test_fixture (`#33 <https://github.com/ros2/ament_cmake_ros/issues/33>`_)
* Fix rmw_test_fixture DLL import on Windows (`#32 <https://github.com/ros2/ament_cmake_ros/issues/32>`_)
* Fix range for rmw_test_fixture_default port locking (`#31 <https://github.com/ros2/ament_cmake_ros/issues/31>`_)
* Stop loading RMW to load the test fixture (`#30 <https://github.com/ros2/ament_cmake_ros/issues/30>`_)
* Contributors: Scott K Logan

0.14.1 (2025-04-02)
-------------------
* Add 'default' rmw_test_fixture based on domain_coordinator (`#26 <https://github.com/ros2/ament_cmake_ros/issues/26>`_)
* Install run_rmw_isolated executable to lib subdirectory (`#25 <https://github.com/ros2/ament_cmake_ros/issues/25>`_)
* Ignore Ctrl-C in run_rmw_isolated on Windows (`#24 <https://github.com/ros2/ament_cmake_ros/issues/24>`_)
* Resolve windows warnings in rmw_test_fixture (`#22 <https://github.com/ros2/ament_cmake_ros/issues/22>`_)
* Add rmw_test_fixture for supporting RMW-isolated testing (`#21 <https://github.com/ros2/ament_cmake_ros/issues/21>`_)
* Contributors: Alejandro Hernández Cordero, Scott K Logan
