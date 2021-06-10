#!/bin/sh

rm -rf build
mkdir build
cd build

cmake -D CMAKE_INSTALL_PREFIX=./out ../
make
make install

