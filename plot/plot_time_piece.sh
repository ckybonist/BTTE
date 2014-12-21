#!/bin/sh

./genr_plot_dat.py 'time_piece'
/usr/bin/gnuplot -e "dat='output/time_piece.dat'; outfile='output/time_piece.png'; xlabelname='piece'" plot.plg
