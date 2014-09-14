#ifndef _PEERMANAGER_CPP
#define _PEERMANAGER_CPP

#include <cstdlib>

#include "error.h"
#include "piece.h"
#include "random.h"
#include "trans_time.h"
#include "peer_manager.h"

PeerManager::PeerManager(const Args &args) {
    args_ = args;
}

void PeerManager::SelectNeighbors(IPeerSelection &ips) const {
    int *peer_list = nullptr;
    peer_list = ips.ChoosePeers();
    if(peer_list == nullptr) {
        ExitError("Error occured when selecting neighbors.");
    }
    g_peers[2].neighbors = peer_list;
}

/*
 * Check amount of each class' peers
 * is reach the target of distributed-rate
 *
 * args:
 * @amount : current amount of n-class' peers
 * @volume : total amount of peers
 *
 * */
static bool RateEnough(int idx, int amount, int num_peer) {
	int limit = (float)k_dist_rate[idx] / (float)100 * num_peer;
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

	while (flag) {
        target = uniformdist::RangeRand(min, max);

		for (int i = 0; i < set_size; i++) {
			if (target == set[i] && set[i] != 0) {
			//if (target == set[i]) {
                break;
            } else {
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
static int ExcludeNum(const int num, const int min, const int max) {
    int target = uniformdist::RangeRand(min, max);

	if (target == num) {
		ExcludeNum(num, min, max);
	} else {
		return num;
	}
}

/*
 * Alloting all peers with three different
 * transmission time (downlink speed)
 *
 * * */
void PeerManager::AllotTransTime_() const {
	int ex_set[k_num_transtime] = {0};
	int counts[k_num_transtime] = {0};

	for (int i = 0; i < args_.NUM_PEER; i++) {
		//int idx = ExcludeSet(ex_set, NUM_TRANSTIME, 0, 2);
        int idx = uniformdist::RangeRand(0, 2);

		g_peers[i].time_per_piece = k_trans_time[idx];

        /* counter for amount of class,  MUST PLACE ABOVE THE RateEnough() */
		++counts[idx];

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if (RateEnough(idx, counts[idx], args_.NUM_PEER)) {
			if(ex_set[idx] == 0) {
                ex_set[idx] = idx;
            }
		}
	}
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds_() const {
    // TODO
    for (int i = 0; i < args_.NUM_SEED; i++) {
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        g_peers[i].is_seed = true;

        for (int j = 0; j < args_.NUM_PIECE; j++) {
            g_peers[i].pieces[j].set_exist(true);
        }

        //TODO g_peers[i].neighbors = SelectNeighbors();
    }
}

/*
 * Peer ID: NUM_SEED ~ (NUM_SEED + NUM_LEECH - 1),
 * About 50% pieces
 *
 * NOTE: Not every peer have 50% of its picces certainly, decided by prob.
 *
 * * */
void PeerManager::InitLeeches_() const {
    /* control the prob to make it not larger than TARGET_PROB
    const double AVG_TARGET_PROB = 0.5;
    double target_prob = AVG_TARGET_PROB * (double)NUM_PEER;
    */
    const int init_idx = args_.NUM_SEED;  // initial leech number: 0 + NUM_SEED

    std::cout.precision(2);

    for (int i = init_idx; i < args_.NUM_PEER; i++) {
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        double prob_leech = (double)g_rand_num / k_randmax;
        g_rand_num = uniformdist::rand();
        GetPieceByProb(g_peers[i].pieces, prob_leech, args_.NUM_PIECE);
    }
}

void PeerManager::NewPeer(int id) const {
    g_peers[id].in_swarm = true;
    // TODO
}

void PeerManager::CreatePeers() const {
    const int &NUM_PEER = args_.NUM_PEER;

    g_peers = new Peer[NUM_PEER];
    if (g_peers == nullptr) {
        ExitError("Allocate memory of peers is fault!\n");
    }

    // init peers and pieces
    for (int i = 0; i < NUM_PEER; i++) { g_peers[i].id = i; }

    AllotTransTime_();

    //InitSeeds_();

    //InitLeeches_();
}

void PeerManager::DestroyPeers() {
    int num_peer = sizeof(g_peers) / sizeof(int);
    /*
    for (int i = 0; i < num_peer; i++) {
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
