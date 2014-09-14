#ifndef _PEERMANAGER_CPP
#define _PEERMANAGER_CPP

//#include <cstdlib>

#include "error.h"
#include "piece.h"
#include "random.h"
#include "peer_level.h"
#include "peer_manager.h"


PeerManager::PeerManager(const Args &args) {
    args_ = args;
}

void PeerManager::SelectNeighbors(IPeerSelection &ips) const {
    int *peer_list = ips.ChoosePeers();
    if(peer_list == nullptr) {
        ExitError("Error occured when selecting neighbors.");
    }
    //g_peers[2].neighbors = peer_list;
}


/* forward declarations */
static bool RateEnough(const int level, const int amount, const int NUM_PEER);
static int ExcludeSet(const int (&ex_set)[g_k_num_level]);

/*
 * Alloting all peers with three different
 * transmission time (downlink speed)
 *
 * NOTE: Each level has its distribution rate (dist_rate in peer_level.h)
 *
 * * */
void PeerManager::AllotPeerLevel_() const {
	int count[g_k_num_level] = { 0 };
	int ex_set[g_k_num_level] = { 0 };

    const int num_peer = args_.NUM_PEER;
	for(int i = 0; i < num_peer; i++)
    {
		int level = ExcludeSet(ex_set);  // create level for each peer

		g_peers[i].time_per_piece = g_k_peer_level[level-1].trans_time;

		++count[level-1];  // count the amount of class, place above the RateEnough()

		if(RateEnough(level-1, count[level-1], num_peer))
		{
			if(ex_set[level-1] == 0)
				ex_set[level-1] = level;
		}
	}

    for(int i = 0; i < 3; i++) {
        std::cout << "Amount of level " << i + 1 << " peers: "
                  << count[i] << "\n";
    }
    std::cout << "\n\n";
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds_() const {
    for (int i = 0; i < args_.NUM_SEED; i++) {
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        g_peers[i].is_seed = true;

        g_peers[i].pieces = MakePieces(args_.NUM_PIECE);

        for (int j = 0; j < args_.NUM_PIECE; j++) {
            g_peers[i].pieces[j].set_exist(true);
        }

        //TODO : g_peers[i].neighbors = SelectNeighbors();
    }
}

/*
 * Peer ID: NUM_SEED ~ (NUM_SEED + NUM_LEECH),
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
    const int start = args_.NUM_SEED;
    const int end = start + args_.NUM_LEECH;

    std::cout.precision(2);

    std::cout << "Prob of each leech: \n";
    for (int i = start; i < end; i++) {
        g_peers[i].id = i;
        g_peers[i].in_swarm = true;
        g_peers[i].is_leech = true;

        g_peers[i].pieces = MakePieces(args_.NUM_PIECE);

        double prob_leech = (double)(uniformdist::rand()) / g_k_randmax;

        std::cout << prob_leech << "\n";

        GetPieceByProb(g_peers[i].pieces, prob_leech, args_.NUM_PIECE);
    }
    std::cout << "\n";
}

void PeerManager::NewPeer(int id) const {
    g_peers[id].in_swarm = true;
    // TODO
}

void PeerManager::CreatePeers() const {
    const int &NUM_PEER = args_.NUM_PEER;

    // DEBUG
    for(int i = 0; i < g_k_num_level; i++) {
        switch(i) {
            case 0:
                std::cout << "Transmission time of level "<< i << " : " << g_k_peer_level[i].trans_time << "\n";
                break;
            case 1:
                std::cout << "Transmission time of level "<< i << " : " << g_k_peer_level[i].trans_time << "\n";
                break;
            case 2:
                std::cout << "Transmission time of level "<< i << " : " << g_k_peer_level[i].trans_time << "\n";
                break;
        }
    }
    std::cout << "\n";

    g_peers = new Peer[NUM_PEER];
    if (g_peers == nullptr) {
        ExitError("Allocating memory of peers is fault!\n");
    }

    /* init seeds, leeches and their pieces */
    for (int i = 0; i < NUM_PEER; i++) { g_peers[i].id = i; }
    AllotPeerLevel_();
    InitSeeds_();
    InitLeeches_();
}

void PeerManager::DestroyPeers() {
    for (int i = 0; i < args_.NUM_PEER; i++) {
        //delete g_peers[i].pgdelay;
        //g_peers[i].pgdelay = nullptr;

        if(g_peers[i].pieces != nullptr) {
            delete g_peers[i].pieces;
            g_peers[i].pieces = nullptr;
        }

        //delete g_peers[i].neighbors;
        //g_peers[i].neighbors = nullptr;
    }

    delete [] g_peers;
    g_peers = nullptr;
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
//static bool RateEnough(const int level, const int amount, const int NUM_PEER) {
static bool RateEnough(const int level, const int amount, const int NUM_PEER) {
	const int limit = static_cast<int>(g_k_peer_level[level].dist_rate * NUM_PEER);
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
static int ExcludeSet(const int (&ex_set)[g_k_num_level]) {
	int target = 0;
	bool flag = true;
    const int k_min = 1;  // NOTE: don't use 0, it will duplicate with loop counter
    const int k_max = g_k_num_level;

	while(flag) {
		//target = roll(k_min, k_max);
		target = uniformdist::RangeRand(k_min, k_max);

		for(int i = 0; i < g_k_num_level; i++) {
			//if(target == ex_set[i] && ex_set[i] != 0)
			if(target == ex_set[i]) {
				flag = true;
				break;
			} else {
				flag = false;
			}
		}
	}

	return target;
}

#endif // for #ifndef _PEERMANAGER_CPP
