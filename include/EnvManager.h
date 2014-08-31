#ifndef __INITENV_H__
#define __INITENV_H__

#include "Piece.h"

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

/* Download speed of each class */
const int DOWNSPEED_1 = 10 * 1024 * 1024;
const int DOWNSPEED_2 = 128 * 1024;
const int DOWNSPEED_3 = 56 * 1024;

/* Distributed rate(%) of each class */
const int DRATE[3] = { 30, 50, 20 };

const float TRANS_TIME[3]  // each class' transmission time
{
	(float)PIECE_SIZE / (float)DOWNSPEED_1,
	(float)PIECE_SIZE / (float)DOWNSPEED_2,
	(float)PIECE_SIZE / (float)DOWNSPEED_3,
};


class PeerManager;

// Singleton Class
// Tracker and Other Else Manager
class EnvManager
{
public:
	static EnvManager &getInstance();

	void init(const PeerManager &pm) const;

    void destroy() const;

private:
	EnvManager() {};
	EnvManager(const EnvManager&);
	void operator=(const EnvManager&);
    ~EnvManager() {};
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
