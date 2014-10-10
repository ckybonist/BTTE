//#include <cstdlib>

#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "peer_manager.h"


static bool RateEnough(const int level, const int amount, const int NUM_PEER);
static int NewPeerLevel(const int (&exclude_set)[g_kNumLevel], const RSC& rsc);


PeerManager::PeerManager(const Args& args)
{
    this->args_ = args;

    const int NUM_PEERLIST = args_.NUM_PEERLIST;
    const int NUM_PEER = args_.NUM_PEER;

    switch (args_.TYPE_PEERSELECT)
    {
        case static_cast<int>(PeerSelect_T::STANDARD) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new Standard(NUM_PEERLIST, NUM_PEER));
            break;

        case static_cast<int>(PeerSelect_T::LOAD_BALANCING) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new LoadBalancing(NUM_PEERLIST, NUM_PEER));
            break;

        case static_cast<int>(PeerSelect_T::CLUSTER_BASED) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new ClusterBased(NUM_PEERLIST, NUM_PEER));
            break;

        default:
            ExitError("Error of casting to interface-class: IPeerSelect");
            break;
    }
}

PeerManager::~PeerManager()
{
    for (int i = 0; i < args_.NUM_PEER; i++)
    {
        delete [] g_peers[i].pieces;
        g_peers[i].pieces = nullptr;
    }

    // also call the destructor to delete neighbors
    delete type_peerselect_; // MUST BEFORE THE DELETION OF G_PEERS

    delete [] g_peers;

    delete [] packet_tt_4_peers;

    type_peerselect_ = nullptr;
    g_peers = nullptr;
    packet_tt_4_peers = nullptr;
}

// for average peers
void PeerManager::AllotNeighbors(const int peer_id) const
{
    Neighbor* neighbors = type_peerselect_->SelectNeighbors();
    g_peers[peer_id].neighbors = neighbors;
}

void PeerManager::NewPeer(const int pid,
                          const int cid,
                          const float join_time) const
{
    g_peers[pid] = Peer(pid,
                        cid,
                        packet_tt_4_peers[pid],
                        join_time,
                        args_.NUM_PIECE);
}

void PeerManager::AllocAllPeersSpaces()
{
    g_peers  = new Peer[args_.NUM_PEER];
    if (g_peers == nullptr)
    {
        ExitError("Allocating memory of peers is fault!\n");
    }
}

void PeerManager::CreatePeers()
{
    // DEBUG
    for(int i = 0; i < g_kNumLevel; i++)
    {
        std::cout << "Bandwidth of level "<< i << " : "
                  << g_kPeerLevel[i].bandwidth << "\n";
    }
    std::cout << "\n";

    // create empty peers
    AllocAllPeersSpaces();

    // init seeds, leeches and their pieces
    const int aborigine = args_.NUM_SEED + args_.NUM_LEECH;

    AllotPeerLevel();

    InitSeeds();

    InitLeeches();

    for(int pid = args_.NUM_SEED; pid < args_.NUM_LEECH; pid++)
    {
        for(int c = 0; c < args_.NUM_PIECE; c++)
            g_all_pieces_get &= g_peers[pid].pieces[c];
    }

    // test of peer join
    for(int pid = aborigine; pid < args_.NUM_PEER; ++pid)
    {
        float time = pid / static_cast<float>(100);

        int cid = uniformrand::Roll<int>(RSC::FREE_5, 1, 4);
        //int cid = uniformrand::Roll(14, 1, 4);

        NewPeer(pid, cid, time);

        AllotNeighbors(pid);
    }

}

void PeerManager::AllotPeerLevel()
{
    const int NUM_PEER = args_.NUM_PEER;

    packet_tt_4_peers = new float[NUM_PEER];

    if(nullptr == packet_tt_4_peers) {
        ExitError("Memory Allocation Fault");
    }


	int count[g_kNumLevel] = { 0 };
	int exclude_set[g_kNumLevel] = { 0 };

    float time_piece[g_kNumLevel] = { 0 };
    for(int i = 0; i < g_kNumLevel; i++)
    {
        time_piece[i] = g_kPieceSize / g_kPeerLevel[i].bandwidth;
    }

	for(int pid = 0; pid < NUM_PEER; pid++)
    {
		int level = NewPeerLevel(exclude_set, RSC::PEER_LEVEL);

        packet_tt_4_peers[pid] = time_piece[level-1];

		++count[level-1];

		if(RateEnough(level-1,
                      count[level-1],
                      args_.NUM_PEER))
        {
			if(exclude_set[level-1] == 0)
				exclude_set[level-1] = level;
		}
	}

    for(int i = 0; i < args_.NUM_PEER; i++)
    {
        std::cout << packet_tt_4_peers[i] << "\n";
    }
    std::cout << "\n\n";

    for(int i = 0; i < 3; i++)
    {
        std::cout << "Amount of level " << i + 1 << " peers: "
                  << count[i] << "\n";
    }

    std::cout << "\n\n";
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds() const
{
    for (int pid = 0; pid < args_.NUM_SEED; pid++)
    {
        Neighbor* neighbors = type_peerselect_->SelectNeighbors();

        g_peers[pid] = Peer(pid,
                            packet_tt_4_peers[pid],
                            neighbors,
                            args_.NUM_PIECE);
    }
}

/*
 * Peer ID: NUM_SEED ~ (NUM_SEED + NUM_LEECH),
 *
 * Important var:
 *      @ prob_leech : prob. of each leech ( [0.1, 0.9] )
 *      @ prob_piece : prob. of each piece (in peer.cpp)
 *
 * Decision method of whether each leech will get each piece or not:
 *      if (prob_piece < prob_leech)
 *          Get piece
 *
 * NOTE: Not every peer have 50% of its picces certainly, decided by prob.
 *
 * * */
void PeerManager::InitLeeches() const
{
    std::cout.precision(3);

    std::cout << "Prob. of each leech: \n";

    using std::cout;
    using std::endl;

    const int kStart = args_.NUM_SEED;
    const int kEnd = kStart + args_.NUM_LEECH;

    for (int pid = kStart; pid < kEnd; pid++)
    {

        double prob_leech = 0;
        prob_leech = uniformrand::Roll<float>(
                     RSC::PROB_LEECH,
                     (float)0.1,
                     (float)0.9);

        Neighbor* neighbors = type_peerselect_->SelectNeighbors();

        g_peers[pid] = Peer(pid,
                            packet_tt_4_peers[pid],
                            neighbors,
                            args_.NUM_PIECE,
                            prob_leech);

        std::cout << prob_leech << "\n";
    }

    std::cout << "============================\n";
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
static bool RateEnough(const int level,
                       const int amount,
                       const int NUM_PEER)
{
	const int limit = static_cast<int>(g_kPeerLevel[level].dist_rate *
                                         NUM_PEER);
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
static int NewPeerLevel(const int (&exclude_set)[g_kNumLevel],
                        const RSC& rsc)
{
	int target = 0;
	bool flag = true;
    const int min = 1;  // NOTE: don't use 0, it will duplicate with loop counter
    const int max = g_kNumLevel;

	while(flag)
    {
		target = uniformrand::Roll<int>(rsc, min, max);

		for(int i = 0; i < g_kNumLevel; i++)
        {
			//if(target == ex_set[i] && ex_set[i] != 0)
			if(target == exclude_set[i])
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

// peer_manager
