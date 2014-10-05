//#include <cstdlib>

#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "peer_manager.h"


static bool PeerEnough(const int NUM_PEER, const int k_cur);
static bool RateEnough(const int k_level, const int k_amount, const int NUM_PEER);
static int NewPeerLevel(const int (&exclude_set)[g_k_num_level], const RSC& k_seed_rsc_id);


PeerManager::PeerManager(const Args& args)
{
    this->_args = args;

    const int NUM_PEERLIST = _args.NUM_PEERLIST;
    const int NUM_PEER = _args.NUM_PEER;

    switch (_args.TYPE_PEERSELECT)
    {
        case static_cast<int>(PeerSelect_T::STANDARD) :
            _type_peerselect =
                static_cast<IPeerSelect*>(new Standard(NUM_PEERLIST, NUM_PEER));
            break;

        case static_cast<int>(PeerSelect_T::LOAD_BALANCING) :
            _type_peerselect =
                static_cast<IPeerSelect*>(new LoadBalancing(NUM_PEERLIST, NUM_PEER));
            break;

        case static_cast<int>(PeerSelect_T::CLUSTER_BASED) :
            _type_peerselect =
                static_cast<IPeerSelect*>(new ClusterBased(NUM_PEERLIST, NUM_PEER));
            break;

        default:
            ExitError("Error of casting to interface-class: IPeerSelect");
            break;
    }
}

PeerManager::~PeerManager()
{
    for (int i = 0; i < _args.NUM_PEER; i++)
    {
        delete [] g_peers[i].pieces;
    }

    // also call the destructor to delete neighbors
    delete _type_peerselect; // MUST BEFORE THE DELETION OF G_PEERS

    delete [] g_peers;

    delete [] _peers_bandwidth;
}

// for average peers
void PeerManager::AllotNeighbors(const int k_peer_id) const
{
    Neighbor* neighbors = _type_peerselect->SelectNeighbors();
    g_peers[k_peer_id].neighbors = neighbors;
}

void PeerManager::NewPeer(const int k_id,
                          const int k_cid,
                          const float k_start_time) const
{
    g_peers[k_id] = Peer(k_id, k_cid,
                       _peers_bandwidth[k_id],
                       k_start_time,
                       _args.NUM_PIECE);
}

void PeerManager::AllocAllPeersSpaces()
{
    g_peers  = new Peer[_args.NUM_PEER];
    if (g_peers == nullptr)
    {
        ExitError("Allocating memory of peers is fault!\n");
    }
}

void PeerManager::CreatePeers()
{
    // DEBUG
    for(int i = 0; i < g_k_num_level; i++)
    {
        std::cout << "Transmission time of level "<< i << " : "
                  << g_k_peer_level[i].trans_time << "\n";
    }
    std::cout << "\n";

    // create empty peers
    const int NUM_PEER = _args.NUM_PEER;

    AllocAllPeersSpaces();

    // init seeds, leeches and their pieces
    const int k_aborigine = _args.NUM_SEED + _args.NUM_LEECH;

    AllotPeerLevel();

    InitSeeds();

    InitLeeches();

    for(int pid = _args.NUM_SEED; pid < _args.NUM_LEECH; pid++)
    {
        for(int c = 0; c < _args.NUM_PIECE; c++)
            g_all_pieces_get &= g_peers[pid].pieces[c];
    }

    // test of peer join
    //for(int pid = k_aborigine; pid < NUM_PEER; ++pid)
    //{
    //    float time = pid / static_cast<float>(100);

    //    int cid = uniformrand::Roll<int>(RSC::FREE_5, 1, 4);
    //    //int cid = uniformrand::Roll(14, 1, 4);

    //    NewPeer(pid, cid, time);

    //    AllotNeighbors(pid);
    //}

}

void PeerManager::AllotPeerLevel()
{
    const int NUM_PEER = _args.NUM_PEER;

    _peers_bandwidth = new float[NUM_PEER];

    if(nullptr == _peers_bandwidth) {
        ExitError("Memory Allocation Fault");
    }

	int count[g_k_num_level] = { 0 };
	int exclude_set[g_k_num_level] = { 0 };

	for(int pid = 0; pid < NUM_PEER; pid++)
    {
		int level = NewPeerLevel(exclude_set, RSC::PEER_LEVEL);

		_peers_bandwidth[pid] = g_k_peer_level[level-1].trans_time;

		++count[level-1];

		if(RateEnough(level-1,
                      count[level-1],
                      _args.NUM_PEER))
        {
			if(exclude_set[level-1] == 0)
				exclude_set[level-1] = level;
		}
	}

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
    for (int pid = 0; pid < _args.NUM_SEED; pid++)
    {
        Neighbor* neighbors = _type_peerselect->SelectNeighbors();

        g_peers[pid] = Peer(pid,
                _peers_bandwidth[pid],
                neighbors,
                _args.NUM_PIECE);
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
    const int k_start = _args.NUM_SEED;
    const int k_end = k_start + _args.NUM_LEECH;

    std::cout.precision(3);

    std::cout << "Prob. of each leech: \n";

    using std::cout;
    using std::endl;
    for (int pid = k_start; pid < k_end; pid++)
    {

        double prob_leech = 0;
        prob_leech = uniformrand::Roll<float>(
                     RSC::PROB_LEECH,
                     (float)0.1,
                     (float)0.9);

        Neighbor* neighbors = _type_peerselect->SelectNeighbors();

        g_peers[pid] = Peer(
                      pid,
                      _peers_bandwidth[pid],
                      neighbors,
                      _args.NUM_PIECE,
                      prob_leech);

        std::cout << prob_leech << "\n";
    }

    std::cout << "============================\n";
}



static bool PeerEnough(const int NUM_PEER, const int k_cur)
{
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
static bool RateEnough(const int k_level,
                       const int k_amount,
                       const int NUM_PEER)
{
	const int k_limit = static_cast<int>(g_k_peer_level[k_level].dist_rate *
                                         NUM_PEER);
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
static int NewPeerLevel(const int (&exclude_set)[g_k_num_level],
                        const RSC& k_seed_rsc_id)
{
	int target = 0;
	bool flag = true;
    const int k_min = 1;  // NOTE: don't use 0, it will duplicate with loop counter
    const int k_max = g_k_num_level;

	while(flag)
    {
		target = uniformrand::Roll<int>(k_seed_rsc_id, k_min, k_max);

		for(int i = 0; i < g_k_num_level; i++)
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
