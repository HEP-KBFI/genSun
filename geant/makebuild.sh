#!/bin/bash
echo "Creating the default build in build/"
rm -R build/
mkdir build/
cd build
cmake ../src/ -DCMAKE_INSTALL_PREFIX=../install
