#!/bin/sh

./genr_plot_dat.py 'time_peer'
/usr/bin/gnuplot -e "dat='output/time_peer.dat'; outfile='output/time_peer.png'; xlabelname='peer'" plot.plg
