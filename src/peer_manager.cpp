#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "peer_manager.h"


static bool RateEnough(const int level, const int amount, const int NUM_PEER);
static int NewPeerLevel(const int (&exclude_set)[g_kNumLevel], const RSC& rsc);

PeerManager::PeerManager()
{
    args_ = nullptr;
    packet_time_4_peers_ = nullptr;
    type_peerselect_ = nullptr;
}

PeerManager::PeerManager(Args* const args)
{
    this->args_ = args;

    switch (args_->TYPE_PEERSELECT)
    {
        case static_cast<int>(PeerSelect_T::STANDARD) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new Standard(*args_));
            break;

        case static_cast<int>(PeerSelect_T::LOAD_BALANCING) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new LoadBalancing(*args_));
            break;

        case static_cast<int>(PeerSelect_T::CLUSTER_BASED) :
            type_peerselect_ =
                static_cast<IPeerSelect*>(new ClusterBased(*args_));
            break;

        default:
            ExitError("Error of casting to interface-class: IPeerSelect");
            break;
    }

    const int dummy_peers = static_cast<int>(args_->NUM_PEER * dummy_peers_rate);
    std::cout << "\nNumber of Dummy Peers: " << dummy_peers << "\n";
    //args_->NUM_PEER += dummy_peers;
}

PeerManager::~PeerManager()
{
    std::cout << "\nDestructor of PeerManager\n";
    for (size_t pid = 0; pid < args_->NUM_PEER; pid++)
    {
        delete [] g_peers[pid].pieces;
        g_peers[pid].pieces = nullptr;
    }

    // also call the destructor to delete neighbors
    delete type_peerselect_; // MUST BEFORE THE DELETION OF G_PEERS
    type_peerselect_ = nullptr;

    //delete [] g_peers;
    //g_peers = nullptr;
    g_peers.clear();
    in_swarm_set_.clear();

    delete [] g_in_swarm_set;
    g_in_swarm_set = nullptr;

    delete [] packet_time_4_peers_;
    packet_time_4_peers_ = nullptr;

    args_ = nullptr;
}


void PeerManager::NewPeer(const int pid,
                          const int cid,
                          const float join_time) const
{
    Peer peer(pid,
              cid,
              join_time,
              packet_time_4_peers_[pid],
              args_->NUM_PIECE);
    g_peers.push_back(peer);
}

void PeerManager::CheckInSwarm(const ISF isf, const int pid)
{
    if (g_in_swarm_set == nullptr)
    {
        g_in_swarm_set = new bool[args_->NUM_PEER];
        if (g_in_swarm_set == nullptr)
        {
            ExitError("Memory Allocation Error");
        }
        else
        {
            for(size_t i = 0; i < args_->NUM_PEER; i++)
                g_in_swarm_set[i] = false;
        }
    }

    if (isf == ISF::JOIN)
    {
        in_swarm_set_.insert(pid);
        g_in_swarm_set[pid] = true;
    }
    else if (isf == ISF::LEAVE)
    {
        in_swarm_set_.erase(pid);
        g_in_swarm_set[pid] = false;
    }
}

// for average peers
void PeerManager::AllotNeighbors(const int peer_id) const
{
    Neighbor* neighbors = type_peerselect_->SelectNeighbors(peer_id, in_swarm_set_);
    g_peers[peer_id].neighbors = neighbors;
}

void PeerManager::AllocAllPeersSpaces()
{
    //g_peers = new Peer[args_->NUM_PEER];
    //if (g_peers == nullptr)
    //{
    //    ExitError("Allocating memory of peers is fault!\n");
    //}
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

    ////// Allocate memroy space for all peers (if use typical array)
    //AllocAllPeersSpaces();

    ///// init seeds, leeches and their pieces
    AllotAllPeersLevel();

    InitSeeds();

    InitLeeches();

    /// A boolean value to check. All pieces are get if it's true
    for(size_t pid = args_->NUM_SEED; pid < args_->NUM_LEECH; pid++)
    {
        for(size_t c = 0; c < args_->NUM_PIECE; c++)
            g_all_pieces_get &= g_peers[pid].pieces[c];
    }
}

void PeerManager::AllotAllPeersLevel()
{
    const size_t NUM_PEER = args_->NUM_PEER;

    packet_time_4_peers_ = new float[NUM_PEER];

    if (nullptr == packet_time_4_peers_)
    {
        ExitError("Memory Allocation Fault");
    }


	int count[g_kNumLevel] = { 0 };
	int exclude_set[g_kNumLevel] = { 0 };
    float time_piece[g_kNumLevel] = { 0 };

    for (int i = 0; i < g_kNumLevel; i++)
    {
        time_piece[i] = (float)g_kPieceSize / g_kPeerLevel[i].bandwidth;
    }

	for (size_t pid = 0; pid < NUM_PEER; pid++)
    {
		int level = NewPeerLevel(exclude_set, RSC::PEER_LEVEL);

        packet_time_4_peers_[pid] = time_piece[level-1];

		++count[level-1];

		if (RateEnough(level-1,
                      count[level-1],
                      args_->NUM_PEER))
        {
			if (exclude_set[level-1] == 0)
				exclude_set[level-1] = level;
		}
	}

    std::cout << "\n\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Amount of level " << i + 1 << " peers: "
                  << count[i] << "\n";
    }

    std::cout << "\n\n";
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds() const
{
    for (size_t pid = 0; pid < args_->NUM_SEED; pid++)
    {
        //g_peers[pid] = Peer(pid, args_->NUM_PIECE);
        Peer seed(static_cast<int>(pid),
                  -1,
                  packet_time_4_peers_[pid],
                  args_->NUM_PIECE);
        g_peers.push_back(seed);
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
void PeerManager::InitLeeches()
{
    std::cout.precision(3);

    std::cout << "Prob. of each leech: \n";

    using std::cout;
    using std::endl;

    const size_t start = args_->NUM_SEED;
    const size_t end = start + args_->NUM_LEECH;

    for (size_t pid = start; pid < end; pid++)
    {
        double prob_leech = 0.0;
        prob_leech = uniformrand::Roll<float>(RSC::PROB_LEECH,
                                              0.1,
                                              0.9);
        Peer leech = Peer(static_cast<int>(pid),
                          packet_time_4_peers_[pid],
                          args_->NUM_PIECE,
                          prob_leech);
        g_peers.push_back(leech);

        CheckInSwarm(ISF::JOIN, pid);

        std::cout << prob_leech << "\n";
    }

    //CheckInSwarm();

    // TODO: bugs in peer selection
    for(size_t pid = start; pid < end; pid++)
    {
        Neighbor* neighbors = type_peerselect_->SelectNeighbors(pid, in_swarm_set_);
        g_peers[pid].neighbors = neighbors;
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
	const int limit =
              static_cast<int>(g_kPeerLevel[level].dist_rate * NUM_PEER);

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

	while (flag)
    {
		target = uniformrand::Roll<int>(rsc, min, max);

		for (int i = 0; i < g_kNumLevel; i++)
        {
			//if(target == ex_set[i] && ex_set[i] != 0)
			if (target == exclude_set[i])
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
