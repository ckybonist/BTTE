#!/bin/sh

# Check arguments
if [ $# -ge 1 ]; then
    BUILD_FLAG=$1
    if [[ "$BUILD_FLAG" != "debug" &&
          "$BUILD_FLAG" != "release" &&
          "$BUILD_FLAG" != "rebuild" &&
          "$BUILD_FLAG" != "makeall" ]]; then
        echo "First Argument: debug, relase, makeall, rebuild \"\" "
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

if [ $BUILD_FLAG == "rebuild" ]; then
    rm -rf $BUILD_DIR
    mkdir $BUILD_DIR
    mkdir $BUILD_DIR/debug $BUILD_DIR/release
    $PROJ_ROOT/script/init_build.sh
elif [ $BUILD_FLAG == "makeall" ]; then
    cd $BUILD_DIR/debug
    make clean && make -j4
    cd ../release
    make clean && make -j4
else
    cd $BUILD_DIR/$BUILD_FLAG
    make clean && make -j4
fi
