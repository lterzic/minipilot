from setuptools import setup, find_packages

setup(
    name="minipilot",
    version="0.1.0",
    packages=find_packages(),
    install_requires=[
        "numpy>=2.2.6",
        "matplotlib>=3.10.5",
        "sympy>=1.14",
        "protobuf==5.29.4",
        "notebook>=7.4.5",
        "pybind11==3.0.1",
        "jupytext"
    ],
)