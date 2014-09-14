#ifndef _TRANSTIME_H
#define _TRANSTIME_H

#include "piece.h"

/**
 * Simulation arguments
 *
 * Piece:
 *     @piece_size: 512 (Kbytes)
 *
 * Category of peers:
 *	   @class1 : 10 (Mbps)   // seeder
 *	   @class2 : 128 (Kbps)  // leecher or normal peer
 *	   @class3 : 56 (Kbps)   // leecher or normal peer
 *
 * */

const int k_num_transtime = 3;

/* Download speed of each class */
const int k_down_speeds[k_num_transtime] = {
    10 * 1024 * 1024,
    128 * 1024,
    56 * 1024
};

/* Distributed rate(%) of each class */
const int k_dist_rate[3] = {30, 50, 20};

const float k_trans_time[k_num_transtime] = { // transmission time of each class
	(float)k_piece_size / (float)k_down_speeds[0],
	(float)k_piece_size / (float)k_down_speeds[1],
	(float)k_piece_size / (float)k_down_speeds[2]
};

#endif // for #ifndef _TRANSTIME_H
