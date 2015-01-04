#!/bin/sh

PROJ_ROOT=$(readlink -f $(pwd))
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(dirname $PROJ_ROOT)
fi

# Check cmd line arguments
# Check CMakeLists exist
if [ ! -f "$PROJ_ROOT/CMakeLists.txt" ]; then
    echo 'CMakeLists Not Found!'
    exit 1
fi

# generating Unix Makefiles
BUILD_DIR="$PROJ_ROOT/build"
cd $BUILD_DIR

# debug
cd debug
cmake -DCMAKE_BUILD_TYPE=debug -G 'Unix Makefiles' $PROJ_ROOT
make

# release
cd ../release
cmake -DCMAKE_BUILD_TYPE=release -G 'Unix Makefiles' $PROJ_ROOT
make
