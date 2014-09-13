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

const int NUM_TRANSTIME = 3;

/* Download speed of each class */
const int DOWNSPEED_1 = 10 * 1024 * 1024;
const int DOWNSPEED_2 = 128 * 1024;
const int DOWNSPEED_3 = 56 * 1024;

/* Distributed rate(%) of each class */
const int DRATE[3] = {30, 50, 20};

const float TRANS_TIME[3] = { // transmission time of each class
	(float)PIECE_SIZE / (float)DOWNSPEED_1,
	(float)PIECE_SIZE / (float)DOWNSPEED_2,
	(float)PIECE_SIZE / (float)DOWNSPEED_3,
};

#endif // for #ifndef _TRANSTIME_H
