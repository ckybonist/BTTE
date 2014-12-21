#!/bin/sh

./genr_plot_dat.py 'time_piece'
/usr/bin/gnuplot -e "dat_prefix='output/time_piece_'; outfile='output/time_piece.png'; xlabelname='piece'" plot.plg
