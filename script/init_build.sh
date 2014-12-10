#!/bin/sh

# Check cmd line arguments
# Check CMakeLists exist
if [ ! -f 'CMakeLists.txt' ]; then
    echo 'CMakeLists Not Found!'
    exit 1
fi


# generating Unix Makefiles
BUILD_DIR='build'
cd $BUILD_DIR

# debug
cd debug
cmake -DCMAKE_BUILD_TYPE=debug -G 'Unix Makefiles' '../..'
make

# release
cd ../release
cmake -DCMAKE_BUILD_TYPE=release -G 'Unix Makefiles' '../..'
make
