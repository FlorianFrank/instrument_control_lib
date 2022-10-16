#!/bin/bash
sudo ../build.sh
sudo rm -rf build
sudo rm -rf py_instrument_control_lib.egg-info
pip install .
