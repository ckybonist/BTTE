#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import os
import sys
import glob
from math import floor
from collections import OrderedDict

def GetPeerAvgCompleteTime(peer_stat):
    num_peer = len(peer_stat)
    total_complete_time = 0
    for data in peer_stat:
        print("{0} {1}".format(data[2], data[1]))
        total_complete_time += data[2] - data[1]
    avg_time = total_complete_time / num_peer
    return floor(avg_time)

def ElementToInt(mylst):
    return list(map(lambda s: float(s), mylst))

def GetVariantBase(fname, plot_type):
    pattern = re.compile("[0-9]+")
    match = None
    if plot_type == 'time_piece':
       #match = re.search("[0-9]+", fname[1]).group(0)
       match = pattern.search(fname[1]).group(0)
       #return int(pattern.search(fname[1]))
    elif plot_type == 'time_peer':
       #match  = re.search("[0-9]+", fname[0]).group(0)
       match = pattern.search(fname[0]).group(0)
    return int(match)


if __name__ == "__main__":
    plot_type = sys.argv[1]
    files = glob.glob(plot_type + "/*")

    tmp_dict = dict()
    variant_base = None

    for f in files:
        with open(f, 'r') as fp:
            f = os.path.basename(f).split('_')
            variant_base = GetVariantBase(f, plot_type)
            peer_stat = [ s.strip().split()
                          for s in fp.readlines() if '#' not in s ]
            peer_stat = [ ElementToInt(e) for e in peer_stat ]
            avg_time = GetPeerAvgCompleteTime(peer_stat)
            tmp_dict[variant_base] = avg_time

    result = OrderedDict(sorted(tmp_dict.items()))  # sort by key

    outfile = 'output/' + plot_type + '.dat'
    with open(outfile, 'a') as fp:
        for num_piece, avg_time in result.items():
            fp.write("{0}\t{1}\n".format(num_piece, avg_time))

# End of File
