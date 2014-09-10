#ifndef _PEERMANAGER_CPP
#define _PEERMANAGER_CPP

#include <cstdlib>

#include "error.h"
#include "piece.h"
#include "random.h"
#include "trans_time.h"
#include "peer_manager.h"

/*
 * Check amount of each class' peers
 * is reach the target of distributed-rate
 *
 * args:
 * @amount : current amount of n-class' peers
 * @volume : total amount of peers
 *
 * */
static bool RateEnough(int cls, int amount, int num_peer) {
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
static int ExcludeSet(int *set, const int set_size,
                      const int min, const int max) {
	int target = 0;
	bool flag = true;

	while(flag) {
        target = uniformdist::RangeRand(min, max);

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
    int target = uniformdist::RangeRand(min, max);

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

void PeerManager::InitArgs(const int num_peer, const int num_seed,
                         const int num_leech, const int num_piece) {
    NUM_PEER = num_peer;
    NUM_SEED = num_seed;
    NUM_LEECH = num_leech;
    NUM_PIECE = num_piece;
}

void PeerManager::SetNeighbors(IPeerSelection &ips) const {
    int *temp = nullptr;
    temp = &(ips.ChoosePeers());
    if(temp == nullptr) {
        ExitWithError("Error occured when selecting neighbors.");
    }
    g_peers[2].neighbors = temp;
}

void PeerManager::AllotTransTime() const {
    // TODO Is this function really random, STILL NEED TESTING
    // Classify peers to three classes
	int ex_set[NUM_TRANSTIME] = {0};
	int counts[NUM_TRANSTIME] = {0};
    int idx = 0;

	for(int i = 0; i < NUM_PEER; i++) {
		idx = ExcludeSet(ex_set, NUM_TRANSTIME, 0, 2);

        if(g_peers == nullptr) {
            ExitWithError("Peers is a nullptr"); // it's true
        }

		g_peers[i].time_per_piece = TRANS_TIME[idx];

		counts[idx] = counts[idx] + 1;  // count of amount of class, this must place before the rateEnough() check

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if(RateEnough(idx, counts[idx], NUM_PEER)) {
			if(ex_set[idx] == 0) { ex_set[idx] = idx; }
		}
	}
}

void PeerManager::InitSeeds() const
{
    // TODO
    // 100 seeds, 100% pieces, probably have three different speeds
    for(int i = 0; i < NUM_SEED; i++) {
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        g_peers[i].is_seed = true;

        for(int j = 0; j < NUM_PIECE; j++) {
            g_peers[i].pieces[j].set_exist(true);
        }
    }
}

void PeerManager::InitLeeches() const
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
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        double prob_leech = (double)g_rand_num / RANDMAX;
        g_rand_num = uniformdist::rand();
        GetPieceByProb(g_peers[i].pieces, prob_leech, NUM_PIECE);
    }
}

void PeerManager::NewPeer(int id) const {
    g_peers[id].in_swarm = true;
    // TODO
}

void PeerManager::CreatePeers() const {
    g_peers = new Peer[NUM_PEER];
    if(g_peers == nullptr) {
        ExitWithError("Allocate memory of peers is fault!\n");
    }

    // init peers and pieces
    for(int i = 0; i < NUM_PEER; i++) { g_peers[i].id = i; }

    AllotTransTime();

    //InitSeeds();

    //InitLeeches();
}

void PeerManager::DestroyPeers() {
    int num_peer = sizeof(g_peers) / sizeof(int);
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

    delete [] g_peers;
    g_peers = nullptr;
}
#endif // for #ifndef _PEERMANAGER_CPP
