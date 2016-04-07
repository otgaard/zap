#!/usr/bin/env bash

echo 'Fetching Submodules'

echo 'git submodule update —init —recursive'

echo 'Building GLFW'

BUILD_DIR=$PWD
echo $BUILD_DIR

mkdir $BUILD_DIR/glfw/build
cd $BUILD_DIR/glfw/build
cmake .. -DCMAKE_INSTALL_PREFIX=$BUILD_DIR
make -j 4
make install
echo 'Building Google Test'
mkdir $BUILD_DIR/googletest/build
cd $BUILD_DIR/googletest/build
cmake .. -DCMAKE_INSTALL_PREFIX=$BUILD_DIR
make -j 4
make install
echo 'Third Party Build Complete'
