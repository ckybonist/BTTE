//#include <cstdlib>

#include "random.h"
#include "error.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "peer_manager.h"

/* forward declarations */
static bool PeerEnough(const int NUM_PEER, const int k_cur);
static bool RateEnough(const int k_level, const int k_amount, const int NUM_PEER);
static int ExcludeSet(const int (&ex_set)[g_k_num_level], const RSC &k_seed_id);
//static int ExcludeSet(const int (&ex_set)[g_k_num_level], const int k_seed_id);


PeerManager::PeerManager(const Args &args) {
    args_ = args;
}

PeerManager::~PeerManager() {
    delete [] peers_bandwidth;
}

void PeerManager::SelectNeighbors(IPeerSelection &ips, const int peer_id) const {
    Neighbor *neighbors = ips.ChoosePeers();
    if(nullptr == neighbors) {
        ExitError("Error occured when selecting neighbors.");
    }
    g_peers[peer_id].neighbors = neighbors;
}

void PeerManager::AllotPeerLevel_() {
    const int NUM_PEER = args_.NUM_PEER;

    peers_bandwidth = new float[NUM_PEER];
    if(nullptr == peers_bandwidth) {
        ExitError("Memory Allocation Fault");
    }

	int count[g_k_num_level] = { 0 };
	int ex_set[g_k_num_level] = { 0 };

	for(int i = 0; i < NUM_PEER; i++) {
		int level = ExcludeSet(ex_set, rsc_peer_level);  // create level for each peer
		//int level = ExcludeSet(ex_set, 0);  // create level for each peer

		peers_bandwidth[i] = g_k_peer_level[level-1].trans_time;

		++count[level-1];  // count the amount of class, place above the RateEnough()

		if(RateEnough(level-1, count[level-1], args_.NUM_PEER)) {
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

        // third arg was nullptr because peer selection not implement yet
        //TODO : Neighbor *neighbors = SelectNeighbors();
        Neighbor *neighbors = nullptr;
        g_peers[i] = Peer(i, peers_bandwidth[i], nullptr, args_.NUM_PIECE);  // 3rd was nullptr until peer selection have completed
    }
}

/*
 * Peer ID: NUM_SEED ~ (NUM_SEED + NUM_LEECH),
 *
 * Important var:
 *      @ prob_leech : prob. of each leech (0.1 ~ 0.9)
 *      @ prob_piece : prob. of each piece (in peer.cpp)
 *
 * Decision method of whether each leech will get each piece or not:
 *      if (prob_piece < prob_leech)
 *          Get piece
 *
 * NOTE: Not every peer have 50% of its picces certainly, decided by prob.
 *
 * * */
void PeerManager::InitLeeches_() const {
    const int k_start = args_.NUM_SEED;
    const int k_end = k_start + args_.NUM_LEECH;

    std::cout.precision(3);

    std::cout << "Prob of each leech: \n";
    for (int i = k_start; i < k_end; i++) {

        double prob_leech = 0;
        while(1) {
            prob_leech = (uniformrand::rand(rsc_prob_leech)) / (double)g_k_rand_max;
            //prob_leech = (uniformrand::rand(1)) / (double)g_k_rand_max;

            if(prob_leech >= 0.1 && prob_leech <= 0.9) {
                break;
            } else {
                continue;
            }
        }

        // third arg was nullptr because peer selection not implement yet
        //TODO : Neighbor *neighbors = SelectNeighbors();
        Neighbor *neighbors = nullptr;
        g_peers[i] = Peer(i, peers_bandwidth[i], neighbors, args_.NUM_PIECE, prob_leech);

        std::cout << prob_leech << "\n";
    }
    std::cout << "\n";
}

void PeerManager::NewPeer(const int id, const int cid, const float start_time) const {
    g_peers[id] = Peer(id, cid,
                       peers_bandwidth[id], start_time,
                       args_.NUM_PIECE);
}

void PeerManager::CreatePeers() {
    // DEBUG
    for(int i = 0; i < g_k_num_level; i++) {
        switch(i) {
            case 0:
                std::cout << "Transmission time of level "<< i << " : "
                          << g_k_peer_level[i].trans_time << "\n";
                break;
            case 1:
                std::cout << "Transmission time of level "<< i << " : "
                          << g_k_peer_level[i].trans_time << "\n";
                break;
            case 2:
                std::cout << "Transmission time of level "<< i << " : "
                          << g_k_peer_level[i].trans_time << "\n";
                break;
        }
    }
    std::cout << "\n";

    // create empty peers
    const int NUM_PEER = args_.NUM_PEER;

    g_peers = new Peer[NUM_PEER];
    if (g_peers == nullptr) {
        ExitError("Allocating memory of peers is fault!\n");
    }

    /* init seeds, leeches and their pieces */
    const int k_aborigine = args_.NUM_SEED + args_.NUM_LEECH;
    AllotPeerLevel_();
    InitSeeds_();
    InitLeeches_();

    //int cur_peer_id = k_aborigine;

    /* test joining of normal peers*/
    for(int i = k_aborigine; i < NUM_PEER; ++i) {
        float time = i / static_cast<float>(100);

        int cid = uniformrand::Roll(rsc_free_5, 1, 4);
        //int cid = uniformrand::Roll(14, 1, 4);

        NewPeer(i, cid, time);
        g_peers[i].neighbors = nullptr;  // temp, until peer selection have been implemented
    }
}

void PeerManager::DestroyPeers() {
    for (int i = 0; i < args_.NUM_PEER; i++) {
        if(nullptr != g_peers[i].pieces) {
            delete [] g_peers[i].pieces;
        }

        if(nullptr != g_peers[i].neighbors) {
            delete [] g_peers[i].neighbors;
        }
    }

    delete [] g_peers;
}



static bool PeerEnough(const int NUM_PEER, const int k_cur) {
    return (NUM_PEER == k_cur);
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
static bool RateEnough(const int k_level, const int k_amount, const int NUM_PEER) {
	const int k_limit = static_cast<int>(g_k_peer_level[k_level].dist_rate * NUM_PEER);
	return (k_amount == k_limit);
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
static int ExcludeSet(const int (&ex_set)[g_k_num_level], const RSC &k_seed_id) {
//static int ExcludeSet(const int (&ex_set)[g_k_num_level], const int k_seed_id) {
	int target = 0;
	bool flag = true;
    const int k_min = 1;  // NOTE: don't use 0, it will duplicate with loop counter
    const int k_max = g_k_num_level;

	while(flag) {
		target = uniformrand::Roll(k_seed_id, k_min, k_max);

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

// peer_manager
