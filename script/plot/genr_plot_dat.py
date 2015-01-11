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
        else:
            time_usage = data[2] - data[1]
            total_complete_time += time_usage
    num_peer = len(simu_record)
    avg_time = total_complete_time / (num_peer - num_seed)
    return round(avg_time, 2)

def GetAvgSimuTime(all_simu_records):
    total_simu = len(all_simu_records)
    total_avg_time = 0.000
    for simu_record in all_simu_records:
        total_avg_time += GetPeerAvgCompleteTime(simu_record)
    avg_avg_time = total_avg_time / total_simu
    return round(avg_avg_time, 2)

def GetVariantFactor(fname, plot_type):
    """
        Get variat-factor(number of piece/peer) from filename
    """
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

def TidyUpRawData(files, plot_type, have_diff_rseed_simu = False):
    result = defaultdict(list)
    all_simu_records = []

    for f in files:
        with open(f, 'r') as fp:
            " If same have multiple simulations with \
              same combination of arguments "
            if have_diff_rseed_simu:
                all_simu_records = [ s.strip().split()
                                     for s in fp.readlines() if '#' not in s ]
                all_simu_records = MergeSimuRecord(all_simu_records)
                avg_time = GetAvgSimuTime(all_simu_records)
            else:
                simu_record = [ ElementToFloat(s.strip().split())
                                     for s in fp.readlines() if '#' not in s ]
                avg_time = GetPeerAvgCompleteTime(simu_record)

            f = os.path.basename(f).split('_')  # [1000, 64, "ClusterBased"]
            simu_algo = os.path.splitext(f[2])[0]
            variant_factor = GetVariantFactor(f, plot_type)
            record = (variant_factor, avg_time)
            result[simu_algo].append(record)
    return result

def ProcessRawData(logdir, plot_type):
    #files = glob.glob("../../log/" + plot_type + "/*")
    files = glob.glob(logdir + 'raw/*')
    result = TidyUpRawData(files, plot_type, have_diff_rseed_simu = True)
    return result


if __name__ == "__main__":
    proj_root = os.getcwd()
    if (os.path.basename(proj_root) != 'BTTE'):
        proj_root = re.search('.*BTTE/', proj_root).group(0)
    else:
        proj_root += '/'

    plot_type = sys.argv[1]  # dir prefix
    logdir = proj_root + 'log/'
    result = ProcessRawData(logdir, plot_type)
    variant_factor = plot_type.split('_')[1]  # piece/peer

    for k in result.keys():
        result[k] = sorted(result[k])

    for simu_algo, record_lst in result.items():
        #outfile = outdir + 'time_piece_' + simu_algo + '.dat'
        outfile = logdir + simu_algo + '.dat'
        with open(outfile, 'w') as fp:
            fp.write("# {0}\ttime\n".format(variant_factor))
            for vf_value, avg_time in record_lst:  # vf : variant factor
                fp.write("{0}\t{1}\n".format(vf_value, avg_time))

# End of File
