#!/bin/sh

# Check arguments
if [ $# -ge 1 ]; then
    COMPILE_FLAG=$1
    if [[ "$COMPILE_FLAG" != "debug" && "$COMPILE_FLAG" != "release" ]]; then
        echo "Give me \"debug\" or \"release\" as first argument otherwise empty string"
        exit 1
    fi

    if [ $# -eq 2 ]; then
        CLEAN_FLAG=$2
        if [ "$CLEAN_FLAG" != "" ]; then
            if [ "$CLEAN_FLAG" != "clean" ]; then
                echo "Give me \"clean\" as \
                      second argument otherwise empty string"
                exit 1
            fi
        fi
    fi
else
    echo "Wrong number of arguments"
    exit 1
fi

# Build project
BUILD_DIR='build'
if [ ! -d $BUILD_DIR ]; then
    ./init_project $COMPILE_FLAG
else
    cd $BUILD_DIR/$COMPILE_FLAG
    if [ "$CLEAN_FLAG" != "" ]; then
        make clean
    fi
    make
fi

