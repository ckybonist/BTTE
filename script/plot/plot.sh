#!/bin/sh

PLOT_TYPE=$1  # peer/piece to time
ALGO_TYPE=$2  # peer/piece selection
OUTDIR='../../log/'
RAWLOG_DIR='raw'

function printHelpMsg() {
    echo 'Usage : ./plot.sh <plot_type> <algo_type>'
    echo '    plot_type : time_piece, time_peer'
    echo '    algo_type : piece_sel, peer_sel'
}

function error() {
    printHelpMsg
    exit 1
}

# Check argc
if [ $# -ne 2 ]; then
    error
fi

# Create argument string of gnuplot
if [ $1 == "time_piece" ]; then
    #python3 genr_plot_dat.py $PLOT_TYPE
    #ARGSTR="dat_prefix=${RAWLOG_DIR}; outfile='${OUTPUT}/time_piece.png'; xlabelname='piece'; algo='${ALGO_TYPE}'"
    ARGSTR="outdir='$OUTDIR'; outfile='time_piece.png'; xlabelname='piece'; algo='$ALGO_TYPE'"
    #/usr/bin/gnuplot -e $ARGSTR plot.plg
elif [ $1 == "time_peer" ]; then
    #python3 genr_plot_dat.py $1
    #ARGSTR="dat_prefix=${RAWLOG_DIR}; outfile='${OUTPUT}/time_peer.png'; xlabelname='peer'; algo='${ALGO_TYPE}'"
    ARGSTR="outdir='$OUTDIR'; outfile='time_peer.png'; xlabelname='peer'; algo='$ALGO_TYPE'"
    #/usr/bin/gnuplot -e $ARGSTR lot.plg
else
    error
fi

# create csv-like data file
python3 genr_plot_dat.py $PLOT_TYPE

# plot graph
/usr/bin/gnuplot -e "$ARGSTR" graph.plg
