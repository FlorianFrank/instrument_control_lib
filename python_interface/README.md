# Build python interface

Run the following commands to build python interfaces lib

In root directory
```
instrument_control_lib$ git submodule update --init --recursive
instrument_control_lib$ cd boost
instrument_control_lib/boost$ ./bootstrap.sh --with-python-version=3.8
instrument_control_lib/boost$ .b2 --with-python
```
