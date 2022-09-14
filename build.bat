#!/bin/bash

rm -rf out
rm -rf build
mkdir build
mkdir build/lib

pushd build/lib
cmake ../.. -D CMAKE_INSTALL_PREFIX=../../out 
make
make install
popd

