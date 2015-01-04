#!/bin/sh

# Use for two cases:
#    1. init project
#    2. new source files added

BUILD_DIR='build'
SCRIPT_DIR='script'

if [ -d $BUILD_DIR ]; then
    rm -rf $BUILD_DIR
fi

$SCRIPT_DIR/init_dirs.sh && \
    $SCRIPT_DIR/init_build.sh
