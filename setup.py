# Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

from setuptools import setup, find_packages

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext


__version__ = '0.1'

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        'pymmcmb',
        sources=[
            'src/binding.cpp'
        ],
        libraries=[
            'mmcmb'
        ],
    ),
]

setup(
    name="pymmcmb",
    version=__version__,
    author="Patrick Huesmann",
    author_email="patrick.huesmann@desy.de",
    url="msk.desy.de",
    description="Python binding for libmmcmb",
    ext_modules=ext_modules,
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={
        "build_ext": build_ext
    },
    packages=find_packages(exclude=['tests']),
    package_data={
        package: ["**/*.pyi"] for package in find_packages(exclude=['tests'])
    },
    zip_safe=False,
)

