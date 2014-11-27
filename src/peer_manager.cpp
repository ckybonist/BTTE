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

namespace
{

bool IsDupDest(const IntSet& dest_peers,
               const int nid)
{
    bool flag = false;
    for (const auto& it : dest_peers)
    {
        if (it == nid)
        {
            flag = true;
            break;
        }
    }
    return flag;
}

bool HavePiece(const int pid, const int piece_no)
{
    return g_peers.at(pid).get_nth_piece(piece_no);
}

std::map<int, IntSet> GetEachPieceOwners(IntSet const& target_pieces,
                                         NeighborMap const& neighbors)
{
    std::map<int, IntSet> piece_owner_map;

    for (const int no : target_pieces)
    {
        for (auto& nei : neighbors)
        {
            if (HavePiece(nei.first, no))
            {
                piece_owner_map[no].insert(nei.first);
            }
        }
    }

    return piece_owner_map;
}

MsgQueue GetUndupDestReqMsgs(IntSet const& target_pieces, const int client_pid)
{
    MsgQueue req_msgs;
    IntSet dest_peers;
    NeighborMap const& neighbors = g_peers.at(client_pid).get_neighbors();
    std::map<int, IntSet> piece_owners_map = GetEachPieceOwners(target_pieces, neighbors);

    size_t i = 0;
    for (const int piece_no : target_pieces)
    {
        ++i;
        auto& neighbors = g_peers.at(client_pid).get_neighbors();
        const int size = neighbors.size(); //for (int i = 0; i < size; i++)

        // If more than one neighbors having this piece,
        // then randomly choose one neighbor to request.
        IntSet const& owners = piece_owners_map.at(piece_no);
        const int dest_pid = RandChooseSetElement(RSC::RF_PIECESELECT, owners);

        if (!IsDupDest(dest_peers, dest_pid))
        {
            PieceMsg msg;
            msg.src_pid = client_pid;
            msg.dest_pid = dest_pid;
            msg.piece_no = piece_no;
            msg.src_up_bw = g_peers.at(client_pid).get_bandwidth().uplink;
            req_msgs.push_back(msg);

            dest_peers.insert(dest_pid);
        }
    }

    return req_msgs;
}

}


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
        g_peers.at(p).destroy_pieces();

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

void PeerManager::NewPeerData(Peer::Type type,
                              const int pid,
                              //const float join_time,
                              double prob_leech) const
{
    const int NUM_PIECE = args_->NUM_PIECE;
    const int cid = reserved_cids_[pid];
    const int level = reserved_peer_levels_[pid];
    Bandwidth const& bw = g_kPeerLevel[level].bandwidth;

    g_peers.push_back(Peer(type, pid, cid, bw));
    Peer& peer = g_peers.at(pid);

    switch (type)
    {
        case Peer::SEED:
        case Peer::NORMAL:
            peer.InitPieces(type, NUM_PIECE);
            break;

        case Peer::LEECH:
            peer.InitPieces(type, NUM_PIECE, prob_leech);
            break;

        default:
            ExitError("Error in NewPeerData(...)");
            break;
    }
}


void PeerManager::NewPeer(const int pid) const
{
    NewPeerData(Peer::NORMAL, pid);
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
            for(size_t i = 0; i < args_->NUM_PEER; i++)
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
    for (size_t c = 0; c < args_->NUM_PIECE; c++)
    {
        if (!g_peers.at(pid).get_nth_piece(c))
        {
            flag = false;
            break;
        }
    }

    return flag;
}

void PeerManager::AllotNeighbors(const int pid) const
{
    //Neighbor* neighbors = obj_peerselect_->StartSelection(pid, in_swarm_set_);
    Peer& client = g_peers.at(pid);
    client.set_neighbors(obj_peerselect_->StartSelection(pid, in_swarm_set_));
}

MsgQueue PeerManager::GetAvailablePieceReqs(const int client_pid)
{
    IntSet target_pieces = obj_pieceselect_->StartSelection(client_pid);
    MsgQueue req_msgs = GetUndupDestReqMsgs(target_pieces, client_pid);

    // debug
    for (const PieceMsg& msg : req_msgs)
    {
        Peer& client = g_peers.at(client_pid);
        Peer& peer = g_peers.at(msg.dest_pid);
        client.push_req_msg(msg);
        peer.push_recv_msg(msg);

        // debug
        std::cout << "Piece Reqest Msg:" << std::endl;
        std::cout << "   src: " << msg.src_pid << std::endl;
        std::cout << "   dest: " << msg.dest_pid << std::endl;
        std::cout << "   wanted piece: " << msg.piece_no << std::endl;
        std::cout << "   src upload bandwidth: " << msg.src_up_bw << "\n\n";
    }

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
        const float up_bandwidth = g_kPeerLevel[i].bandwidth.downlink;
        std::cout << "Transmission Time of level "<< i << " : "
                  << g_kPieceSize / up_bandwidth << std::endl;
    }
    std::cout << "\n\n\n";

    // Allocate memroy space for all peers (array)
    //AllocPeersSpace();
    InitSeeds();
    InitLeeches();
}

void PeerManager::DeployPeersLevel()
{
	int count[g_kNumLevel] = { 0 };
	int exclude_set[g_kNumLevel] = { 0 };
    const size_t NUM_PEER = args_->NUM_PEER;

	for (size_t p = 0; p < NUM_PEER; p++)
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
    for (size_t p = 0; p < args_->NUM_PEER; p++)
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
    for (size_t p = 0; p < args_->NUM_SEED; p++)
    {
        Peer::Type type = Peer::SEED;
        //NewPeerData(type, p, 0.0);
        NewPeerData(type, p);
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

    for (int p = start; p < end; p++)
    {
        double prob_leech = Roll<double>(RSC::PROB_LEECH, 0.1, 0.9);
        //NewPeerData(Peer::LEECH, p, 0.0, prob_leech);
        NewPeerData(Peer::LEECH, p, prob_leech);
        UpdateSwarmInfo(ISF::JOIN, p);
    }

    for(int p = start; p < end; p++) AllotNeighbors(p);

    std::cout << "============================\n";
}
