#include "Piece.h"
#include "Error.h"
#include "Random.h"
#include "TransTime.h"
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

/*
 * Exclude set of number in the random number set
 *
 * Args:
 *     @set : the set of numbers will be excluded
 *     @size : the size of set
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   the value within the specific range but
 *	   without that exclusive set
 *
 * */
static int excludeSet(int const * const set, const int size,
        const int min, const int max) {
	int target = 0;
	bool flag = true;

	while(flag)
    {
        target = uniformdist::range_rand(min, max);

		for(int i = 0; i < size; i++)
        {
			//if(target == set[i] && set[i] != 0)
			if(target == set[i])
		   	{
				flag = true;
				break;
			}
		   	else
		   	{
				flag = false;
			}
		}
	}
	return target;
}

/*
 * Exclude specific number in the random number set
 *
 * Args:
 *     @num : the number be excluded
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   the value within the specific range but
 *	   without exclusive number
 *
 * Note: Currently not using this function. Being reserved.
 *
 * */
static int excludeNum(const int num, const int min, const int max)
{
    int target = uniformdist::range_rand(min, max);

	if(target == num) {
		excludeNum(num, min, max);
	} else {
		return num;
	}
}


PeerManager::PeerManager(const int num_peer, const int num_seed, const int num_leech, const int num_piece)
{
    NUM_PEER = num_peer;
    NUM_SEED = num_seed;
    NUM_LEECH = num_leech;
    NUM_PIECE = num_piece;
}

void PeerManager::setNeighbors(IPeerSelection &ips) const
{
    int *temp = nullptr;
    temp = &(ips.choosePeers());
    if(temp == nullptr) { exitWithError("Error occured when selecting neighbors."); }
    peers[2].neighbors = temp;
}

void PeerManager::allotTransTime() const
{
    // TODO Is this function really random, NEED TESTING
    // Classify peers to three classes
	int exSet[3] = {0};
	int counts[3] = {0};
	int size = sizeof(exSet) / sizeof(*exSet);  // exclusive set size

    //srand(time(0));

	for(int i = 0; i < NUM_PEER; i++)
    {
		int cls = excludeSet(exSet, size, 1, 3);

		peers[i].time_per_piece = TRANS_TIME[cls];

		++counts[cls-1];  // count of amount of class, this must place before the isEnough() check

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if(isEnough(cls-1, counts[cls-1], NUM_PEER))
		{
			if(exSet[cls-1] == 0)
				exSet[cls-1] = cls;
		}
	}
}

void PeerManager::initSeeds() const
{
    // TODO
    // 100 seeds, 100% pieces, probably have three different speeds
}

void PeerManager::initLeeches() const
{
    // TODO
    // 100 leechs, about 50% pieces, probably have three different speeds
}

void PeerManager::newPeer(Peer_t &peer, int id) const
{
    peer.id = id;
    peer.in_swarm = true;
    // TODO
}

void PeerManager::createPeers() const
{
    Peer_t *peers = nullptr;
    peers = new Peer_t[NUM_PEER];
    if(peers == nullptr) { exitWithError("Allocate memory of peers is fault!\n"); }

    // init peers and pieces
    for(int i = 0; i < NUM_PEER; i++)
    {
        peers[i].id = i;
        peers[i].pieces = makePieces(NUM_PIECE);
    }

    allotTransTime();

    initSeeds();

    initLeeches();
}

void PeerManager::destroyPeers(Peer_t *peers)
{
    int num_peer = sizeof(peers) / sizeof(int);
    for(int i = 0; i < num_peer; i++)
    {
        delete peers[i].pgdelay;
        peers[i].pgdelay = nullptr;

        delete peers[i].pieces;
        peers[i].pieces = nullptr;

        delete peers[i].neighbors;
        peers[i].neighbors = nullptr;
    }

    delete [] peers;
    peers = nullptr;
}
