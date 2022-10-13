rmdir /s /q build
mkdir build
mkdir build\lib

pushd build\lib
cmake "-DCMAKE_MAKE_PROGRAM=ninja" -G Ninja -S ..\.. -B .
cmake --build . --target all -j 6
pushd python_interface
move py_instrument_control_lib*.pyd py_instrument_control_lib.pyd
popd
popd
