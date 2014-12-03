#include "error.h"
#include "algo.h"
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

typedef std::vector<int> IntVec;
typedef std::map<int, IntVec> I2IVecMap;

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
    bool have = g_peers.at(pid).get_nth_piece_info(piece_no);
    return have;
}

IntSet GetPieceOwners(const int piece_no, const int client_pid)
{
    IntSet owners;
    Peer const& client = g_peers.at(client_pid);
    for (auto const& nei : client.get_neighbors())
    {
        // NOTE: Not only check weather having this piece, but also
        // check the neighbor is in swarm
        if (HavePiece(nei.first, piece_no) &&
                    g_peers_reg_info[nei.first])
        {
            owners.insert(nei.first);
        }
    }

    IntSet on_req_peers = client.get_on_req_peer_set();

    // TODO 要去除掉正在要求過程中的目標節點
    return owners;
}

IntVec GetNonReqPieceOwners(const int no, const int client_pid)
{
    IntSet owners = GetPieceOwners(no, client_pid);
    IntSet on_req_peers = g_peers.at(client_pid).get_on_req_peer_set();

    IntVec result(owners.size());

    // 從這個 piece 的持有者中再去除掉之前有送過要求但還沒拿到 piece 的持有者，
    // 也就是不要對同一個 neighbor 送超過一個要求。
    btte_set_diff(owners.begin(), owners.end(),
                  on_req_peers.begin(), on_req_peers.end(),
                  result.begin());

    IntVec::iterator it = result.begin();
    for (; it != result.end(); ++it)
    {
        if (*it == 0) break;
    }
    result.erase(it, result.end());

    if (result.size() == 0)
        std::cout << "Owner of all pieces are on request. Can't found possible request\n";
    else
        for (const int pid : result)
            std::cout << "Non Req Piece Owner: " << pid << std::endl;

    return result;
}

std::map<int, IntVec> GetPieceOwnersMap(IntSet const& target_pieces, const int client_pid)
{
    std::map<int, IntVec> piece_owner_map;

    // 蒐集所有 piece 的持有者（已過濾）
    for (const int no : target_pieces)
        piece_owner_map[no] = GetNonReqPieceOwners(no, client_pid);
        //piece_owner_map[no] = GetPieceOwners(no, client_pid);

    return piece_owner_map;
}

MsgList GetUndupDestReqMsgs(IntSet const& target_pieces, const int client_pid)
{
    Peer const& client = g_peers.at(client_pid);
    I2IVecMap piece_owners_map = GetPieceOwnersMap(target_pieces, client_pid);

    MsgList req_msgs;
    IntSet dest_peers;

    // 針對每一個 piece，找到 "不重複" 的持有者，並封裝成一個要求訊息。
    // 最終將每個訊息集合成一組訊息列表
    //
    // NOTE-1: 不一定每個 rarest piece 找到合適的持有者去要求
    // NOTE-2: 不重複有兩種意思:
    //             1. 不跟正在要求的節點重複
    //             2. 不同 pieces 之間的要求對象是互斥的
    for (const int piece_no : target_pieces)
    {
        IntVec const& owners = piece_owners_map.at(piece_no);

        if (owners.size() != 0)
        {
            // If more than one neighbors have this piece,
            // then randomly choose one neighbor to request.
            const int dest_pid = RandChooseElementInContainer(RSC::RF_PIECESELECT, owners);

            // 紀錄
            if (!IsDupDest(dest_peers, dest_pid))
            {
                const float src_up_bw = client.get_bandwidth().uplink;

                PieceMsg msg(client_pid, dest_pid, piece_no, src_up_bw);
                req_msgs.push_back(msg);

                dest_peers.insert(dest_pid);
            }
        }
    }

    dest_peers.clear();

    return req_msgs;
}

}  // anonymous namespace


void PeerManager::InitAbstractObj()
{
    const int TYPE_PEERSELECT = g_btte_args.get_type_peerselect();
    const int TYPE_PIECESELECT = g_btte_args.get_type_pieceselect();

    // 將指定的 peer selection 演算法所屬之衍生類別轉形成基底類別
    const PeerSelect_T type_peer_select = static_cast<PeerSelect_T>(TYPE_PEERSELECT);
    switch (type_peer_select)
    {
        case PeerSelect_T::STANDARD:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new Standard());
            std::cout << "\n----Using Standard Peer Selection----\n\n";
            break;

        case PeerSelect_T::LOAD_BALANCING:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new LoadBalancing());
            std::cout << "\n----Using Load Balancing Peer Selection----\n\n";
            break;

        case PeerSelect_T::CLUSTER_BASED:
            obj_peerselect_ =
                static_cast<IPeerSelect*>(new ClusterBased());
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
            obj_pieceselect_ = static_cast<IPieceSelect*>(new RarestFirst());
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

PeerManager::PeerManager()
{
    // Reserve spaces for vector of peers
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    g_peers.reserve(NUM_PEER);

    // Init regestration of peers
    if (g_peers_reg_info == nullptr)
    {
        g_peers_reg_info = new bool[NUM_PEER];
        if (g_peers_reg_info == nullptr)
           ExitError("Memory Allocation Error");

        for (size_t i = 0; i < NUM_PEER; i++)
            g_peers_reg_info[i] = false;
    }

    // Init object of algorithm execution
    InitAbstractObj();

    // Init member data
    reserved_peer_levels_ = new int[NUM_PEER];
    if (nullptr == reserved_peer_levels_)
        ExitError("Memory Allocation Fault");
    DeployPeersLevel();

    reserved_cids_ = new int[NUM_PEER];
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

    for (size_t p = 0; (size_t)p < g_btte_args.get_num_peer(); p++)
        g_peers.at(p).destroy_pieces();

    // also call the destructor to delete neighbors
    delete obj_peerselect_; // MUST BEFORE THE DELETION OF G_PEERS
    obj_peerselect_ = nullptr;

    delete obj_pieceselect_; // MUST BEFORE THE DELETION OF G_PEERS
    obj_pieceselect_ = nullptr;

    //delete [] g_peers;
    //g_peers = nullptr;
    g_peers.clear();

    delete [] g_peers_reg_info;
    g_peers_reg_info = nullptr;
}

void PeerManager::NewPeerData(Peer::Type type,
                              const int pid,
                              double prob_leech) const
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
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
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    if (isf == ISF::JOIN)
    {
        g_peers_reg_info[pid] = true;
    }
    else if (isf == ISF::LEAVE)
    {
        g_peers_reg_info[pid] = false;
    }
}

bool PeerManager::CheckAllPiecesGet(const int pid) const
{
    bool flag = true;
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        if (!g_peers.at(pid).get_nth_piece_info(c))
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
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    IntSet in_swarm_set;

    for (size_t i = 0; i < NUM_PEER; i++)
    {
        if (g_peers_reg_info[i])
            in_swarm_set.insert(i);
    }

    Peer& client = g_peers.at(pid);
    client.set_neighbors(obj_peerselect_->StartSelection(pid, in_swarm_set));
}

MsgList PeerManager::GenrAllPieceReqs(const int client_pid)
{
    IntSet target_pieces = obj_pieceselect_->StartSelection(client_pid);
    MsgList req_msgs = GetUndupDestReqMsgs(target_pieces, client_pid);

    // DEBUG
    for (PieceMsg const& msg : req_msgs)
    {
        // debug
        std::cout << "Piece Reqest Msg:" << std::endl;
        std::cout << "   src: " << msg.src_pid << std::endl;
        std::cout << "   dest: " << msg.dest_pid << std::endl;
        std::cout << "   wanted piece: " << msg.piece_no << std::endl;
        std::cout << "   src upload bandwidth: " << msg.src_up_bw << "\n\n";
    }

    return req_msgs;
}

// For request timeout situation
PieceMsg PeerManager::ReGenrPieceReq(const int piece_no, const int client_pid)
{
    Peer const& client = g_peers.at(client_pid);
    NeighborMap const& neighbors = client.get_neighbors();

    // Get ownsers of piece which not on request
    std::vector<int> owners = GetNonReqPieceOwners(piece_no, client_pid);

    // Create new msg
    PieceMsg msg;
    if (owners.size() != 0)
    {
        const int dest_pid = RandChooseElementInContainer(RSC::RF_PIECESELECT, owners);
        const float src_up_bw = client.get_bandwidth().uplink;
        msg = PieceMsg(client_pid, dest_pid, piece_no, src_up_bw);
    }

    return msg;
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
    for (int i = 0; i < g_kNumLevel; i++)
    {
        const float up_bandwidth = g_kPeerLevel[i].bandwidth.downlink;
        std::cout << "Transmission Time of level " << i << " : "
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
    const size_t NUM_PEER = g_btte_args.get_num_peer();

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
    const size_t NUM_PEER = g_btte_args.get_num_peer();

    // assing a cid to each peer, and don't exceed
    // the volume (NUM_PEER / g_kNumClusters) of cluster
    for (size_t p = 0; p < NUM_PEER; p++)
    {
        int cid = RangeRandNumExceptEx<int, g_kNumClusters>(RSC::CB_PEERSELECT, exclude_set);
        const int idx = cid - 1;

        reserved_cids_[p] = cid;

        ++count[idx];

        const int kMaxClusterAmount = static_cast<int>(NUM_PEER / (float)g_kNumClusters);

        if (count[idx] == kMaxClusterAmount)
        {
            if (exclude_set[idx] == 0)
                exclude_set[idx] = cid;
        }
    }
}

/* Peer ID: 0 ~ NUM_SEED-1, 100% pieces */
void PeerManager::InitSeeds() const
{
    const size_t NUM_SEED = g_btte_args.get_num_seed();
    for (size_t p = 0; p < NUM_SEED; p++)
    {
        Peer::Type type = Peer::SEED;
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
    const size_t start = g_btte_args.get_num_seed();
    const size_t end = start + g_btte_args.get_num_leech();

    for (size_t p = start; p < end; p++)
    {
        double prob_leech = Roll<double>(RSC::PROB_LEECH, 0.1, 0.9);
        //NewPeerData(Peer::LEECH, p, 0.0, prob_leech);
        NewPeerData(Peer::LEECH, p, prob_leech);
        UpdateSwarmInfo(ISF::JOIN, p);
    }

    for (size_t p = start; p < end; p++)
        AllotNeighbors(p);

    std::cout << "============================\n";
}
