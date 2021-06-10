#!/bin/sh

mkdir build
cd build
cmake ../
make

mkdir out
cd out
cp ../libce_device.so .
cp ../../*.h . 
