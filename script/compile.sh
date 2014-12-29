#!/bin/sh

# Check arguments
if [ $# -ge 1 ]; then
    BUILD_FLAG=$1
    if [[ "$BUILD_FLAG" != "debug" &&
          "$BUILD_FLAG" != "release" &&
          "$BUILD_FLAG" != "all" ]]; then
        echo "First Argument: debug, relase, all, \"\" "
        exit 1
    fi
else
    echo "Wrong number of arguments"
    exit 1
fi

# Build project
PROJ_ROOT=$(readlink -f $(pwd))
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(dirname $PROJ_ROOT)
fi

BUILD_DIR="$PROJ_ROOT/build"

if [ ! -d $BUILD_DIR ]; then
    ./init_project $BUILD_FLAG
else
    if [ $BUILD_FLAG == "all" ]; then
        cd $BUILD_DIR/debug
        make clean && make
        cd ../release
        make clean && make
    else
        cd $BUILD_DIR/$BUILD_FLAG
        make clean && make
    fi
fi

