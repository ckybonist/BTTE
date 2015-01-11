#!/bin/sh

PLOT_TYPE=$1  # peer/piece to time
ALGO_TYPE=$2  # peer/piece selection

#PROJ_ROOT="$(readlink -f $(pwd))/"
PROJ_ROOT="$(pwd)/"
if [ $(basename $PROJ_ROOT) != 'BTTE' ]; then
    PROJ_ROOT=$(echo $PROJ_ROOT | sed -e 's@script/plot@@')
fi

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
    ARGSTR="outdir='${PROJ_ROOT}log/'; outfile='time_piece.png'; xlabelname='piece'; algo='$ALGO_TYPE'"
elif [ $1 == "time_peer" ]; then
    ARGSTR="outdir='${PROJ_ROOT}log/'; outfile='time_peer.png'; xlabelname='peer'; algo='$ALGO_TYPE'"
else
    error
fi

PLOTDIR="${PROJ_ROOT}script/plot"
# create csv-like data file
python3 $PLOTDIR/genr_plot_dat.py $PLOT_TYPE

# plot graph
/usr/bin/gnuplot -e "$ARGSTR" $PLOTDIR/graph.plg
