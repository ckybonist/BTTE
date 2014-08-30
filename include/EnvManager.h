#ifndef __INITENV_H__
#define __INITENV_H__

#include "Peer.h"

/**
 * Simulation arguments
 *
 * Piece:
 *     @piece_size: 512 (Kbytes)
 *
 * Category of peers:
 *	   @class1 : 10 (Mbps)
 *	   @class2 : 128 (Kbps)
 *	   @class3 : 56 (Kbps)
 *
 * */

const int PIECE_SIZE = 512 * 1024 * 8;

/* Download speed of each class */
const int DSPEED_C1 = 10 * 1024 * 1024;
const int DSPEED_C2 = 128 * 1024;
const int DSPEED_C3 = 56 * 1024;

/* Distributed rate(%) of each class */
const int DRATE[3] = { 30, 50, 20 };

const float TRANS_TIME[3]  // each class' transmission time
{
	(float)PIECE_SIZE / (float)DSPEED_C1,
	(float)PIECE_SIZE / (float)DSPEED_C2,
	(float)PIECE_SIZE / (float)DSPEED_C3,
};


// Singleton Class
// Tracker and Other Else Manager
class EnvManager
{
public:
	static EnvManager& getInstance();

	void init();

    void destroy();

private:
	EnvManager() {};
	EnvManager(const EnvManager &);
	void operator=(const EnvManager &);
    ~EnvManager() {};

    Piece& makePieces();
    void allotTransTime(Peer_t&);
    void makeSeeds(Peer&);
    void makeLeechs(Peer&);
    Peer& selectPeers();
    void createPeers();
};

/* Normal
class EnvManager
{
public:
	EnvManager() {};
	void createPeers(int*, int);

};
*/

#endif
