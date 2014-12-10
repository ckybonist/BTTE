#!/bin/sh

BUILD="build"

if [ ! -d $BUILD ]; then
    mkdir -p $BUILD/debug $BUILD/release
fi
