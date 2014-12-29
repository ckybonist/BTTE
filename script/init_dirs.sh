#!/bin/sh

PROJ_ROOT=$(readlink -f $(pwd))
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(dirname $PROJ_ROOT)
fi

BUILD_DIR="$PROJ_ROOT/build"

if [ ! -d $BUILD_DIR ]; then
    mkdir -p $BUILD_DIR/debug $BUILD_DIR/release
fi
