#!/bin/sh

./genr_plot_dat.py 'time_peer'
/usr/bin/gnuplot -e "dat_prefix='output/time_peer_'; outfile='output/time_peer.png'; xlabelname='peer'" plot.plg
