#!/usr/bin/env python

from setuptools import setup

package_name = 'domain_coordinator'

setup(
    name=package_name,
    version='0.1.0',
    description='A tool to coordinate unique ROS_DOMAIN_IDs across multiple processes',

    author='Pete Baughman',
    author_email='pete.baughman@apex.ai',

    packages=[
        'domain_coordinator',
    ],
    tests_require=['pytest'],
    zip_safe=True,
)
