from setuptools import find_packages
from setuptools import setup

setup(
    name='lbot_arm_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('lbot_arm_interfaces', 'lbot_arm_interfaces.*')),
)
