#!/bin/sh

if [ -f ../bin/BTTE ]; then
    /usr/bin/valgrind --leak-check=full --track-origins=yes --log-file=leak.log ../bin/BTTE btte.conf.dbg
else
    echo "No binary found"
    exit 1
fi
