#!/bin/sh

PROJ_ROOT=$(readlink -f $(pwd))
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(dirname $PROJ_ROOT)
fi

BUILD_DIR="$PROJ_ROOT/build"
LOG_DIR="$PROJ_ROOT/log"
PLOT_DIR="$PROJ_ROOT/plot/"

if [ ! -d $LOG_DIR ]; then
    mkdir $LOG_DIR
fi

if [ ! -d $PLOT_DIR/output ]; then
    mkdir $PLOT_DIR/output
fi

if [ ! -d $PLOT_DIR/time_piece ]; then
    mkdir $PLOT_DIR/time_piece
fi

if [ ! -d $PLOT_DIR/time_peer ]; then
    mkdir $PLOT_DIR/time_peer
fi

if [ ! -d $BUILD_DIR ]; then
    mkdir -p $BUILD_DIR/debug $BUILD_DIR/release
fi
