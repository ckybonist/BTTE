#include "Dice.h"

#include "Config.h"

#include "Convert.h"

#include "PeerManager.h"

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

PeerManager::PeerManager(const int num_peer, const int num_seed, const int num_leech)
{
    NUM_PEER = num_peer;
    NUM_SEED = num_seed;
    NUM_LEECH = num_leech;
}

void PeerManager::allotTransTime(Peer_t &peers, int NUM_PEER) const
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

void PeerManager::makeSeeds(Peer_t &peers, int NUM_SEED) const
{
    srand(time(0));
	Dice dice;
}

void PeerManager::makeLeechs(Peer_t &peers, int NUM_LEECH) const
{
    srand(time(0));
	Dice dice;
}

Peer_t& PeerManager::selectPeers() const
{
}

void PeerManager::createPeers () const
{
    Peer_t *peers = new (std::nothrow) Peer_t[NUM_PEER];
    if(peers == nullptr) { exitWithError("Allocate memory of peers is fault!\n"); }

    // init peer id
    for(int i = 0; i < NUM_PEER; i++) { peers->id = Convert::T_to_str(i+1); }

    allotTransTime(*peers, NUM_PEER);

    makeSeeds(*peers, NUM_SEED);

    makeLeechs(*peers, NUM_LEECH);
}
