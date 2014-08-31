#include <iostream>

#include <new>

#include <ctime>

#include "Dice.h"

#include "Error.h"

#include "Peer.h"

#include "EnvManager.h"


/*
 * Check amount of each class' peers
 * is reach the target of distributed-rate
 *
 * args:
 * @amount : current amount of n-class' peers
 * @volume : total amount of peers
 *
 * */
static bool isEnough(int cls, int amount, int num_peer)
{
	int limit = (float)DRATE[cls] / (float)100 * num_peer;
	return (amount == limit) ? (true) : (false);
}


/* Class definition */
EnvManager& EnvManager::getInstance()
{
	static EnvManager instance;
	return instance;
}

void EnvManager::allotTransTime(Peer_t &peers, int NUM_PEER) const
{
    // Classify peers to three classes
	int exSet[3] = {0};
	int counts[3] = {0};
	int size = sizeof(exSet) / sizeof(*exSet);  // exclusive set size

    srand(time(0));
	Dice dice;

	for(int i = 0; i < NUM_PEER; i++)
    {

		int cls = dice.excludeSet(exSet, size, 1, 3);

		peers.time_piece = TRANS_TIME[cls];

		++counts[cls-1];  // count the amount of class, this must place before the isEnough() check

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if(isEnough(cls-1, counts[cls-1], NUM_PEER))
		{
			if(exSet[cls-1] == 0)
				exSet[cls-1] = cls;
		}
	}
}

void EnvManager::init() const
{
    //createPeers();
}

void EnvManager::destroy() const
{
    for(int i = 0; i < num_peer; i++)
    {
        delete peers[i].pdelay;
        peers[i].pdelay = nullptr;

        delete peers[i].pieces;
        peers[i].pieces = nullptr;

        delete peers[i].neighbors;
        peers[i].neighbors = nullptr;
    }

    delete [] peers;
    peers = nullptr;
}
