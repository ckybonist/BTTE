#!/bin/sh

/usr/bin/valgrind --leak-check=full --track-origins=yes --log-file=leak.log ../bin/BTTE btte.conf.dbg
