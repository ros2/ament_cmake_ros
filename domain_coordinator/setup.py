#!/usr/bin/env python

from setuptools import setup

package_name = 'domain_coordinator'

setup(
    name=package_name,
    version='0.7.0',
    description='A tool to coordinate unique ROS_DOMAIN_IDs across multiple processes',
    long_description=(
        'This package provides the functionality used by ament_cmake_pytest_isolated, '
        'ament_cmake_gtest_isolated, and launch_testing to select unique ROS_DOMAIN_IDs '
        'that allow tests to run in parallel without interfering with each other.'
    ),
    license='Apache License, Version 2.0',
    url='https://github.com/ros2/ament_cmake_ros',
    download_url='https://github.com/ros2/ament_cmake_ros/releases',

    author='Pete Baughman',
    author_email='pete.baughman@apex.ai',

    packages=[
        'domain_coordinator',
    ],
    tests_require=['pytest'],
    zip_safe=True,
)
