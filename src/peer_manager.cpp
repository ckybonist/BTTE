#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "cluster_info.h"

#include "STD_peer_selection.h"
#include "LB_peer_selection.h"
#include "CB_peer_selection.h"

#include "RFP_piece_selection.h"
#include "RF_piece_selection.h"

#include "peer_manager.h"


using namespace uniformrand;


PeerManager::PeerManager()
{
    args_ = nullptr;
    reserved_cids_ = nullptr;
    reserved_peer_levels_ = nullptr;
    obj_peerselect_ = nullptr;
}

void PeerManager::InitAbstractObj()
{
    // 將指定的 peer selection 演算法所屬之衍生類別轉形成基底類別
    const PeerSelect_T type_peer_select = static_cast<PeerSelect_T>(args_->TYPE_PEERSELECT);
    switch (type_peer_select)
    {
        case PeerSelect_T::STANDARD:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new Standard(*args_));
            std::cout << "\n----Using Standard Peer Selection----\n\n";
            break;

        case PeerSelect_T::LOAD_BALANCING:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new LoadBalancing(*args_));
            std::cout << "\n----Using Load Balancing Peer Selection----\n\n";
            break;

        case PeerSelect_T::CLUSTER_BASED:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new ClusterBased(*args_));
            std::cout << "\n----Using Cluster Based Peer Selection----\n\n";
            break;

        default:
            ExitError("Error of casting child-class to \
                       interface-class IPeerSelect");
            break;
    }

    //const PieceSelect_T type_piece_select = static_cast<PieceSelect_T>(args_->TYPE_PIECESELECT);
    const PieceSelect_T type_piece_select = static_cast<PieceSelect_T>(0);  // test rarest first, default is random-first-piece
    switch (type_piece_select)
    {
        case PieceSelect_T::BUILTIN:
            //obj_pieceselect_ = static_cast<IPieceSelect*>(new RandomFirstPiece(*args_));
            //std::cout << "\n----Builtin method: Random First Piece Selection----\n\n";
            obj_pieceselect_ = static_cast<IPieceSelect*>(new RarestFirst(*args_));
            std::cout << "\n----Builtin method: Rarest First Selection----\n\n";
            break;
        case PieceSelect_T::USER_DEFINED_1:
            //obj_pieceselect_ =
            //    static_cast<IPieceSelect*>(new UserDefined1(*args_));
            //std::cout << "\n----Using First User Defined Piece Selection----\n\n";
            break;

        case PieceSelect_T::USER_DEFINED_2:
            //obj_pieceselect_ =
            //    static_cast<IPieceSelect*>(new UserDefined2(*args_));
            //std::cout << "\n----Using Second User Defined Piece Selection----\n\n";
            break;

        default:
            ExitError("Error of casting child-class to \
                       interface-class IPeerSelect");
            break;
    }
}

PeerManager::PeerManager(Args* const args)
{
    this->args_ = args;

    // Reserve spaces for vector
    g_peers.reserve(args_->NUM_PEER + 1000);

    InitAbstractObj();

    reserved_peer_levels_ = new int[args->NUM_PEER];
    if (nullptr == reserved_peer_levels_)
        ExitError("Memory Allocation Fault");
    DeployPeersLevel();

    reserved_cids_ = new int[args->NUM_PEER];
    if (nullptr == reserved_cids_)
        ExitError("Memory Allocation Fault");
    DeployClusterIDs();

    // TODO: Add some extra events
    //const int dummy_peers = static_cast<int>(args_->NUM_PEER * dummy_peers_rate);
    //std::cout << "\nNumber of Dummy Peers: " << dummy_peers << "\n";
    //args_->NUM_PEER += dummy_peers;
}

PeerManager::~PeerManager()
{
    std::cout << "\nDestructor of PeerManager\n";

    delete [] reserved_peer_levels_;
    reserved_peer_levels_ = nullptr;

    delete [] reserved_cids_;
    reserved_cids_ = nullptr;

    for (size_t p = 0; (size_t)p < args_->NUM_PEER; p++)
    {
        delete [] g_peers.at(p).pieces;
        g_peers.at(p).pieces = nullptr;
    }

    // also call the destructor to delete neighbors
    delete obj_peerselect_; // MUST BEFORE THE DELETION OF G_PEERS
    obj_peerselect_ = nullptr;

    delete obj_pieceselect_; // MUST BEFORE THE DELETION OF G_PEERS
    obj_pieceselect_ = nullptr;

    //delete [] g_peers;
    //g_peers = nullptr;
    g_peers.clear();
    in_swarm_set_.clear();

    delete [] g_in_swarm_set;
    g_in_swarm_set = nullptr;
    args_ = nullptr;
}

void PeerManager::NewPeerData(PeerType type, const int pid, const float join_time, double prob_leech) const
{
    const int level = reserved_peer_levels_[pid];
    Bandwidth bw = g_kPeerLevel[level].bw;
    const int cid = reserved_cids_[pid];
    const int NUM_PIECE = args_->NUM_PIECE;

    Peer peer;

    switch (type)
    {
        case SEED:
            peer = Peer(pid, cid, NUM_PIECE, bw);
            break;

        case LEECH:
            peer = Peer(pid, cid, NUM_PIECE, prob_leech, bw);
            break;

        case NORMAL:
            peer = Peer(pid, cid, NUM_PIECE, join_time, bw);
            break;

        default:
            ExitError("Error in NewPeerData(...)");
    }

    g_peers.push_back(peer);
}


void PeerManager::NewPeer(const int pid, const float join_time) const
{
    NewPeerData(NORMAL, pid, join_time);
}

void PeerManager::UpdateSwarmInfo(const ISF isf, const int pid)
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
            for(int i = 0; (size_t)i < args_->NUM_PEER; i++)
                g_in_swarm_set[i] = false;
        }
    }

    if (isf == ISF::JOIN)
    {
        in_swarm_set_.insert(pid);
        g_in_swarm_set[pid] = true;
    }
    else
    {
        in_swarm_set_.erase(pid);
        g_in_swarm_set[pid] = false;
    }
}

bool PeerManager::CheckAllPiecesGet(const int pid) const
{
    bool flag = true;
    for (int c = 0; (size_t)c < args_->NUM_PIECE; c++)
    {
        if (!g_peers.at(pid).pieces[c])
        {
            flag = false;
            break;
        }
    }

    return flag;
}

void PeerManager::AllotNeighbors(const int pid) const
{
    Neighbor* neighbors = obj_peerselect_->StartSelection(pid, in_swarm_set_);
    g_peers.at(pid).neighbors = neighbors;
}

std::list<PieceMsg> PeerManager::GetPieceReqMsgs(const int self_pid)
{
    const auto req_msgs = obj_pieceselect_->StartSelection(self_pid);
    return req_msgs;
}

//void PeerManager::AllocPeersSpace()
//{
//    g_peers = new Peer[args_->NUM_PEER];
//    if (g_peers == nullptr)
//    {
//        ExitError("Allocating memory of peers is fault!\n");
//    }
//}

void PeerManager::CreatePeers()
{
    // DEBUG
    for(int i = 0; i < g_kNumLevel; i++)
    {
        std::cout << "Transmission Time of level "<< i << " : "
                  << g_kPieceSize / g_kPeerLevel[i].bw.downlink << "\n";
    }
    std::cout << "\n\n\n";

    /// Allocate memroy space for all peers (if use typical array)
    //AllocPeersSpace();

    InitSeeds();

    InitLeeches();
}

void PeerManager::DeployPeersLevel()
{
    const size_t NUM_PEER = args_->NUM_PEER;

	int count[g_kNumLevel] = { 0 };
	int exclude_set[g_kNumLevel] = { 0 };

	for (int p = 0; (size_t)p < NUM_PEER; p++)
    {
        int level = RangeRandNumExceptEx<int, g_kNumLevel>(RSC::PEER_LEVEL, exclude_set);
        const int idx = level - 1;

        reserved_peer_levels_[p] = idx;

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
    for (int p = 0; (size_t)p < args_->NUM_PEER; p++)
    {
        int cid = RangeRandNumExceptEx<int, g_kNumClusters>(RSC::CB_PEERSELECT, exclude_set);
        const int idx = cid - 1;

        reserved_cids_[p] = cid;

		++count[idx];

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
    for (int p = 0; (size_t)p < args_->NUM_SEED; p++)
        NewPeerData(SEED, p, 0.0);
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

    for (int p = start; p < end; p++)
    {
        double prob_leech = Roll<double>(RSC::PROB_LEECH, 0.1, 0.9);
        NewPeerData(LEECH, p, 0.0, prob_leech);
        UpdateSwarmInfo(ISF::JOIN, p);
    }

    for(int p = start; p < end; p++) AllotNeighbors(p);

    std::cout << "============================\n";
}
