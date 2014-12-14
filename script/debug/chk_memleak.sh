#!/bin/sh

/usr/bin/valgrind --leak-check=full --track-origins=yes --log-file=leak.log ../../build/debug/bin/BTTE btte.conf.dbg > simu.log
