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


mkdir build/bin
pushd build/bin
cmake ../../memristor_tester -D CMAKE_INSTALL_PREFIX=../../out
make
make install
popd

mkdir build/bin
pushd build/bin
cmake ../../instrument_cmd_tool -D CMAKE_INSTALL_PREFIX=../../out
make
make install
popd
