#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    This script compute average complete time
    of each simulation. and generate a *.dat file which
    use for gnuplot

"""

import re
import os
import sys
import glob
#from math import floor
from collections import defaultdict


def ElementToFloat(mylst):
    return list(map(lambda s: float(s), mylst))

def GetPeerAvgCompleteTime(simu_record):
    total_complete_time = 0
    num_seed = 0
    for data in simu_record:
        if (data[2] == 0):  # ignore seeder
            num_seed += 1
        total_complete_time += data[2] - data[1]
    num_peer = len(simu_record)
    avg_time = total_complete_time / (num_peer - num_seed)
    return avg_time

def GetAvgSimuTime(all_simu_records):
    total_simu = len(all_simu_records)
    total_avg_time = 0.000
    print(total_simu)
    for simu_record in all_simu_records:
        total_avg_time += GetPeerAvgCompleteTime(simu_record)
    return round(total_avg_time / total_simu, 2)


def GetVariantFactor(fname, plot_type):
    pattern = re.compile("[0-9]+")
    match = None
    if plot_type == 'time_piece':
       match = pattern.search(fname[1]).group(0)
    elif plot_type == 'time_peer':
       match = pattern.search(fname[0]).group(0)
    return int(match)

def MergeSimuRecord(all_simu_records):
    """
        Merge element of list which belons to same simulation record
        into sub-list.
    """
    result = []
    tmp = []
    for peer_info in all_simu_records:
        if peer_info[0] != ';':
            tmp.append(ElementToFloat(peer_info))
        else:
            result.append(tmp)
            tmp = []
    return result


if __name__ == "__main__":
    plot_type = sys.argv[1]
    files = glob.glob(plot_type + "/*")

    result = defaultdict(list)
    variant_base = None

    all_simu_records = []

    for f in files:
        with open(f, 'r') as fp:
            all_simu_records = [ s.strip().split()
                          for s in fp.readlines() if '#' not in s ]
            all_simu_records = MergeSimuRecord(all_simu_records)

            avg_time = GetAvgSimuTime(all_simu_records)
            f = os.path.basename(f).split('_')
            simu_algo = os.path.splitext(f[2])[0]
            variant_factor = GetVariantFactor(f, plot_type)
            record = (variant_factor, avg_time)
            result[simu_algo].append(record)

    for k in result.keys():
        result[k] = sorted(result[k])

    outdir = 'output/'
    for simu_algo, record_lst in result.items():
        outfile = outdir + 'time_piece_' + simu_algo + '.dat'
        with open(outfile, 'w') as fp:
            for vf, avg_time in record_lst:  # vf : variant factor
                fp.write("{0}\t{1}\n".format(vf, avg_time))

# End of File
