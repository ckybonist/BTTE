#!/bin/sh

BIN = "bin"
LIB = "lib"
OBJ = "obj"

if [ ! -d $BIN ]; then
    mkdir ./bin
fi

if [ ! -d $LIB ]; then
    mkdir ./lib
fi

if [ ! -d $obj ]; then
    mkdir -p ./obj/dbg ./obj/bin
fi
