#!/bin/bash

pushd build/lib
rm playground/playground
cmake ../.. -D CMAKE_INSTALL_PREFIX=../../out 
make
make install
popd


./build/lib/playground/playground
