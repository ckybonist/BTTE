#ifndef _PEERMANAGER_CPP
#define _PEERMANAGER_CPP

#include <cstdlib>

#include "Error.h"
#include "Piece.h"
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
static bool rateEnough(int cls, int amount, int num_peer) {
	int limit = (float)DRATE[cls] / (float)100 * num_peer;
	return (amount == limit);
}

/*
 * Exclude set of number in the random number set
 *
 * Args:
 *     @set : the set of numbers will be excluded
 *     @set_size : the size of set
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   the value within the specific range but
 *	   without that exclusive set
 *
 * */
static int excludeSet(int *set, const int set_size,
                      const int min, const int max) {
	int target = 0;
	bool flag = true;

	while(flag) {
        target = uniformdist::range_rand(min, max);
        //target = range_rand(min, max);

		for(int i = 0; i < set_size; i++) {
			//if(target == set[i] && set[i] != 0)
			if(target == set[i]) { break; }
		   	else { flag = false; }
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
static int excludeNum(const int num, const int min, const int max) {
    int target = uniformdist::range_rand(min, max);

	if(target == num) {
		excludeNum(num, min, max);
	} else {
		return num;
	}
}

/*
PeerManager::PeerManager(const int num_peer, const int num_seed,
                         const int num_leech, const int num_piece) {
    NUM_PEER = num_peer;
    NUM_SEED = num_seed;
    NUM_LEECH = num_leech;
    NUM_PIECE = num_piece;
}
*/

void PeerManager::initArgs(const int num_peer, const int num_seed,
                         const int num_leech, const int num_piece) {
    NUM_PEER = num_peer;
    NUM_SEED = num_seed;
    NUM_LEECH = num_leech;
    NUM_PIECE = num_piece;
}

void PeerManager::setNeighbors(IPeerSelection &ips) const {
    int *temp = nullptr;
    temp = &(ips.choosePeers());
    if(temp == nullptr) {
        exitWithError("Error occured when selecting neighbors.");
    }
    peers[2].neighbors = temp;
}

void PeerManager::allotTransTime() const {
    // TODO Is this function really random, STILL NEED TESTING
    // Classify peers to three classes
    // FIXED Segmenetation fault
	int exSet[NUM_TRANSTIME] = {0};
	int counts[NUM_TRANSTIME] = {0};
    int idx = 0;

	for(int i = 0; i < NUM_PEER; i++) {
		idx = excludeSet(exSet, NUM_TRANSTIME, 0, 2);

        if(peers == nullptr) {
            exitWithError("Peers is a nullptr"); // it's true
        }

		peers[i].time_per_piece = TRANS_TIME[idx];

		counts[idx] = counts[idx] + 1;  // count of amount of class, this must place before the rateEnough() check

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if(rateEnough(idx, counts[idx], NUM_PEER)) {
			if(exSet[idx] == 0) { exSet[idx] = idx; }
		}
	}
}

void PeerManager::initSeeds() const
{
    // TODO
    // 100 seeds, 100% pieces, probably have three different speeds
    for(int i = 0; i < NUM_SEED; i++) {
        peers[i].id = i;
        peers[i].in_swarm = true;
        peers[i].is_seed = true;

        for(int j = 0; j < NUM_PIECE; j++) {
            peers[i].pieces[j].setStatus(true);
        }
    }
}

void PeerManager::initLeeches() const
{
    // TODO
    // 100 leechs, about 50% pieces, probably have three different speeds
    /* control prob. not over TARGET_PROB
    const double AVG_TARGET_PROB = 0.5;
    double target_prob = AVG_TARGET_PROB * (double)NUM_PEER;
    */
    const int init_idx = NUM_SEED;  // initial leech number: 0 + NUM_SEED
    std::cout.precision(2);
    for(int i = init_idx; i < NUM_PEER; i++) {
        peers[i].id = i;
        peers[i].in_swarm = true;
        double prob_leech = (double)randNum / RAND_MAX;
        randNum = uniformdist::rand();
        getPieceByProb(peers[i].pieces, prob_leech, NUM_PIECE);
    }
}

void PeerManager::newPeer(Peer_t *const peers, int id) const {
    peers[id].in_swarm = true;
    // TODO
}

void PeerManager::createPeers() const {
    peers = new Peer_t[NUM_PEER];
    if(peers == nullptr) { exitWithError("Allocate memory of peers is fault!\n"); }

    // init peers and pieces
    for(int i = 0; i < NUM_PEER; i++) { peers[i].id = i; }

    allotTransTime();

    //initSeeds();

    //initLeeches();
}

void PeerManager::destroyPeers(Peer_t *peers) {
    int num_peer = sizeof(peers) / sizeof(int);
    /*
    for(int i = 0; i < num_peer; i++) {
        //delete peers[i].pgdelay;
        //peers[i].pgdelay = nullptr;

        //delete peers[i].pieces;
        //peers[i].pieces = nullptr;

        //delete peers[i].neighbors;
        //peers[i].neighbors = nullptr;
    }
    */

    delete [] peers;
    peers = nullptr;
}
#endif // for #ifndef _PEERMANAGER_CPP
