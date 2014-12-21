#!/bin/sh

PROJ_ROOT=$(readlink -f $(pwd))
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(dirname $PROJ_ROOT)
fi

BUILD_DIR="$PROJ_ROOT/build"
LOG_DIR="$PROJ_ROOT/log"

if [ ! -d $LOG_DIR ]; then
    mkdir $LOG_DIR
fi

if [ ! -d $BUILD_DIR ]; then
    mkdir -p $BUILD_DIR/debug $BUILD_DIR/release
fi
