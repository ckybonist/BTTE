#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "cluster_info.h"
#include "peer_manager.h"

using namespace uniformrand;


PeerManager::PeerManager()
{
    args_ = nullptr;
    packet_time_4_peers_ = nullptr;
    type_peerselect_ = nullptr;
}

PeerManager::PeerManager(Args* const args)
{
    this->args_ = args;

    packet_time_4_peers_ = new float[args->NUM_PEER];
    if (nullptr == packet_time_4_peers_)
        ExitError("Memory Allocation Fault");
    DeployPeersLevel();


    cluster_ids_ = new int[args->NUM_PEER];
    if (nullptr == cluster_ids_)
        ExitError("Memory Allocation Fault");
    DeployClusterIDs();


    // 將指定的 peer selection 演算法所屬之衍生類別轉形成基底類別
    const PeerSelect_T type_ps = static_cast<PeerSelect_T>(args_->TYPE_PEERSELECT);
    switch (type_ps)
    {
        case PeerSelect_T::STANDARD:
            type_peerselect_ =
                static_cast<IPeerSelect*>(new Standard(*args_));
            std::cout << "\n----Using Standard Peer Selection----\n\n";
            break;

        case PeerSelect_T::LOAD_BALANCING:
            type_peerselect_ =
                static_cast<IPeerSelect*>(new LoadBalancing(*args_));
            std::cout << "\n----Using Load Balancing Selection----\n\n";
            break;

        case PeerSelect_T::CLUSTER_BASED:
            type_peerselect_ =
                static_cast<IPeerSelect*>(new ClusterBased(*args_));
            std::cout << "\n----Using Cluster Based Selection----\n\n";
            break;

        default:
            ExitError("Error of casting child-class to \
                       interface-class IPeerSelect");
            break;
    }

    const int dummy_peers = static_cast<int>(args_->NUM_PEER * dummy_peers_rate);
    std::cout << "\nNumber of Dummy Peers: " << dummy_peers << "\n";
    //args_->NUM_PEER += dummy_peers;
}

PeerManager::~PeerManager()
{
    std::cout << "\nDestructor of PeerManager\n";

    delete [] packet_time_4_peers_;
    packet_time_4_peers_ = nullptr;

    delete [] cluster_ids_;
    cluster_ids_ = nullptr;

    for (size_t pid = 0; (size_t)pid < args_->NUM_PEER; pid++)
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
    args_ = nullptr;
}


void PeerManager::NewPeer(const int pid,
                          const float join_time) const
{
    Peer peer(pid,
              cluster_ids_[pid],
              join_time,
              packet_time_4_peers_[pid],
              args_->NUM_PIECE);
    g_peers.push_back(peer);
}

void PeerManager::CheckInSwarm(const ISF isf, const int pid) {
    if (g_in_swarm_set == nullptr)
    {
        g_in_swarm_set = new bool[args_->NUM_PEER];
        if (g_in_swarm_set == nullptr)
        {
            ExitError("Memory Allocation Error");
        }
        else
        {
            for(int i = 0; (size_t)i < args_->NUM_PEER; i++)
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

void PeerManager::AllocPeersSpace()
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

    /// Allocate memroy space for all peers (if use typical array)
    //AllocPeersSpace();

    InitSeeds();

    InitLeeches();

    /// A boolean value to check. All pieces are get if it's true
    for(int pid = args_->NUM_SEED; (size_t)pid < args_->NUM_LEECH; pid++)
    {
        for(int c = 0; (size_t)c < args_->NUM_PIECE; c++)
            g_all_pieces_get &= g_peers[pid].pieces[c];
    }
}

void PeerManager::DeployPeersLevel()
{
    const size_t NUM_PEER = args_->NUM_PEER;

	int count[g_kNumLevel] = { 0 };
	int exclude_set[g_kNumLevel] = { 0 };
    float time_piece[g_kNumLevel] = { 0 };

    for (int i = 0; i < g_kNumLevel; i++)
    {
        time_piece[i] = (float)g_kPieceSize / g_kPeerLevel[i].bandwidth;
    }

	for (int pid = 0; (size_t)pid < NUM_PEER; pid++)
    {
        int level = RangeRandNumExceptEx<int, g_kNumLevel>(RSC::PEER_LEVEL, exclude_set);

        const int idx = level - 1;

        packet_time_4_peers_[pid] = time_piece[idx];

		++count[idx];

        const int max_num = static_cast<int>(g_kPeerLevel[idx].dist_rate * NUM_PEER);

        if (count[idx] == max_num)
        {
			if (exclude_set[idx] == 0)
				exclude_set[idx] = level;
		}
	}

    std::cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Amount of level " << i + 1 << " peers: "
                  << count[i] << "\n";
    }

    std::cout << "\n";
}

void PeerManager::DeployClusterIDs()
{
    int count[g_kNumClusters] = { 0 };
	int exclude_set[g_kNumClusters] = { 0 };

    // assing a cid to each peer, and don't exceed
    // the volume (NUM_PEER / g_kNumClusters) of cluster
    for (int pid = 0; (size_t)pid < args_->NUM_PEER; pid++)
    {
        int cid = RangeRandNumExceptEx<int, g_kNumClusters>(RSC::CB_PEERSELECT, exclude_set);
        const int idx = cid - 1;

        cluster_ids_[pid] = cid;

		++count[idx];

        std::cout.precision(4);
        const int max_cluster_amount = static_cast<int>(args_->NUM_PEER / (float)g_kNumClusters);

        if (count[idx] == max_cluster_amount)
        {
			if (exclude_set[idx] == 0)
				exclude_set[idx] = cid;
		}
    }
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds() const
{
    for (int pid = 0; (size_t)pid < args_->NUM_SEED; pid++)
    {
        //g_peers[pid] = Peer(pid, args_->NUM_PIECE);
        Peer seed(pid,
                  cluster_ids_[pid],
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
    const int start = args_->NUM_SEED;
    const int end = start + args_->NUM_LEECH;

    //std::cout.precision(3);
    //std::cout << "Prob. of each leech: \n";

    for (int pid = start; pid < end; pid++)
    {
        double prob_leech = Roll<double>(RSC::PROB_LEECH,
                                         0.1,
                                         0.9);
        Peer leech = Peer(pid,
                          cluster_ids_[pid],
                          packet_time_4_peers_[pid],
                          args_->NUM_PIECE,
                          prob_leech);
        g_peers.push_back(leech);

        CheckInSwarm(ISF::JOIN, pid);

        //std::cout << prob_leech << "\n";
    }

    // TODO: bugs in peer selection
    for(int pid = start; pid < end; pid++)
    {
        Neighbor* neighbors = type_peerselect_->SelectNeighbors(pid, in_swarm_set_);
        g_peers[pid].neighbors = neighbors;
    }

    std::cout << "============================\n";
}
