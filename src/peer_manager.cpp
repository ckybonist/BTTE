#include <fstream>
#include "error.h"
#include "random.h"

#include "peer.h"
#include "piece.h"
#include "peer_level.h"
#include "cluster_info.h"

#include "algorithm/general_algo.h"
#include "algorithm/std_peer_selection.h"
#include "algorithm/lb_peer_selection.h"
#include "algorithm/cb_peer_selection.h"
#include "algorithm/rfp_piece_selection.h"
#include "algorithm/rf_piece_selection.h"
#include "../custom/my_peer_selection.h"
#include "../custom/my_piece_selection.h"

#include "peer_manager.h"


using namespace btte_uniformrand;


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
    const bool have = g_peers.at(pid).get_nth_piece_info(piece_no);
    return have;
}

IntSet GetPieceOwners(const int piece_no, const int client_pid)
{
    IntSet owners;
    Peer const& client = g_peers.at(client_pid);
    for (auto const& nei : client.get_neighbors())
    {
        const int pid = nei.first;
        // NOTE: Not only check weather having this piece, but also
        // check the neighbor is in swarm
        if (HavePiece(pid, piece_no) &&
                    g_peers_reg_info[pid])
        {
            owners.insert(pid);
        }
    }

    return owners;
}

IntVec GetNonReqPieceOwners(const int no, const int client_pid)
{
    IntSet owners = GetPieceOwners(no, client_pid);

    // Collect peers which on request to a set
    IntSet on_req_peers;
    MsgList send_msg_buf = g_peers.at(client_pid).get_send_msg_buf();
    for (PieceMsg const& msg : send_msg_buf)
    {
        on_req_peers.insert(msg.dest_pid);
    }

    // 從這個 piece 的持有者中再去除掉之前有送過要求但還沒拿到 piece 的持有者，
    // 也就是不要對同一個 neighbor 送超過一個要求。
    IntVec result(owners.size(), -1);
    btte_set_diff(owners.begin(), owners.end(),
                  on_req_peers.begin(), on_req_peers.end(),
                  result.begin());

    // Remove invalid result (equalt to the value which use to init vector)
    IntVec::iterator it = result.begin();
    for (; it != result.end(); ++it)
    {
        if (*it == -1) break;
    }
    result.erase(it, result.end());

    return result;
}

std::map<int, IntVec> GetPieceOwnersMap(IntSet const& target_pieces, const int client_pid)
{
    std::map<int, IntVec> piece_owner_map;

    // 蒐集所有 piece 的持有者（已過濾）
    for (const int no : target_pieces)
        piece_owner_map[no] = GetNonReqPieceOwners(no, client_pid);

    return piece_owner_map;
}


/*
 * 針對每一個 rarest piece，找到 "不重複" 的持有者，封裝成要求訊息，
 * 最終將每個訊息集合成一個列表列表
 *
 * NOTE-1: 不一定能為每個 rarest piece 找到合適的 neighbor 去要求。
 * NOTE-2: 不重複有兩種意思:
 *            1. 不跟正在要求(還在處理中的要求)的 neighbor 重複
 *            2. 要求的 neighbor 是互斥的
 *
 * FIXME: 假設某個 piece(A) 被多個 neighbors 擁有，所以會亂數挑一個去要求，
 *        但有可能挑中的這個 neighbor 是另一個欲要求的 piece(B) 的唯一擁有者。
 *        也就是，原本它可找到擁有 B 的 neighbor，但因為它也是 A 的持有者，所以
 *        我們先向它要求 A 的話，這輪就無法要求 B 了。
 *
 * * * * * * * * * * * * * */
MsgList GetUndupDestReqMsgs(IntSet const& target_pieces, const int client_pid)
{
    Peer const& client = g_peers.at(client_pid);
    I2IVecMap piece_owners_map = GetPieceOwnersMap(target_pieces, client_pid);

    MsgList req_msgs;
    IntSet dest_peers;
    for (const int piece_no : target_pieces)
    {
        IntVec const& owners = piece_owners_map.at(piece_no);

        if (owners.size() != 0)
        {
            // If more than one neighbor have this piece,
            // then randomly choose one neighbor to request.
            const int dest_pid = RandChooseElementInContainer(RSC::PIECESELECT, owners);

            if (!IsDupDest(dest_peers, dest_pid))
            {
                const float pg_delay = client.get_neighbor_pgdelay(dest_pid);
                const float src_up_bw = client.get_bandwidth().uplink;

                PieceMsg msg(client_pid, dest_pid, piece_no, pg_delay, src_up_bw);
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

    // 將指定的 peer selection 演算法所屬之衍生類別轉形成基底類別
    const PeerSelect_t type_peer_select =
                       static_cast<PeerSelect_t>(TYPE_PEERSELECT);

    switch (type_peer_select)
    {
        case PeerSelect_t::STANDARD:
            obj_peerselect_ =
                static_cast<IPeerSelection*>(new Standard());
            break;

        case PeerSelect_t::LOAD_BALANCING:
            obj_peerselect_ =
                static_cast<IPeerSelection*>(new LoadBalancing());
            break;

        case PeerSelect_t::CLUSTER_BASED:
            obj_peerselect_ =
                static_cast<IPeerSelection*>(new ClusterBased());
            break;

        case PeerSelect_t::USER_DEFINED:
            obj_peerselect_ =
                static_cast<IPeerSelection*>(new MyPeerSelection());
            break;

        default:
            ExitError("Error of casting child-class to \
                       interface-class IPeerSelect");
            break;
    }


    const int TYPE_PIECESELECT = g_btte_args.get_type_pieceselect();
    const PieceSelect_t type_piece_select =
                        static_cast<PieceSelect_t>(TYPE_PIECESELECT);  // rarest_first

    switch (type_piece_select)
    {
        case PieceSelect_t::RANDOM:
            obj_pieceselect_ =
                    static_cast<IPieceSelection*>(new RandomFirstPiece());
            break;

        case PieceSelect_t::RAREST_FIRST:
            obj_pieceselect_ =
                    static_cast<IPieceSelection*>(new RarestFirst());
            break;

        case PieceSelect_t::USER_DEFINED:
            obj_pieceselect_ =
                    static_cast<IPieceSelection*>(new MyPieceSelection());
            break;

        default:
            ExitError("Error of casting child-class to \
                       interface-class IPeerSelect");
            break;
    }
}

PeerManager::PeerManager()
{
    const size_t NUM_PEER = g_btte_args.get_num_peer();

    /* Init object of algorithm execution */
    InitAbstractObj();

    /* Init member data */
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
    delete [] reserved_peer_levels_;
    reserved_peer_levels_ = nullptr;

    delete [] reserved_cids_;
    reserved_cids_ = nullptr;

    for (size_t p = 0; (size_t)p < g_btte_args.get_num_peer(); p++)
        g_peers.at(p).destroy_pieces();

    delete obj_peerselect_; // MUST BEFORE THE DELETION OF g_peers (if it is array)
    obj_peerselect_ = nullptr;

    delete obj_pieceselect_; // MUST BEFORE THE DELETION OF g_peers (if it is array)
    obj_pieceselect_ = nullptr;

    //delete [] g_peers;
    //g_peers = nullptr;
    g_peers.clear();

    delete [] g_peers_reg_info;
    g_peers_reg_info = nullptr;
}

void PeerManager::NewPeerData(Peer::Type type,
                              const int pid,
                              double prob_leech)
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
    const int cid = reserved_cids_[pid];
    const int level = reserved_peer_levels_[pid];
    Bandwidth const& bw = g_kPeerLevel[level].bandwidth;

    g_peers.push_back(Peer(type, pid, cid, bw));
    UpdatePeerRegStatus(ISF::JOIN, pid);

    Peer& peer = g_peers.at(pid);
    switch (type)
    {
        case Peer::SEED:
        case Peer::NORMAL:
            peer.InitPieces(type, NUM_PIECE);
            break;

        case Peer::LEECH:
            peer.InitPieces(type, NUM_PIECE, prob_leech);
            AllotNeighbors(peer.get_pid());
            break;

        default:
            ExitError("Error in NewPeerData(...)");
            break;
    }
}


void PeerManager::NewPeer(const int pid)
{
    NewPeerData(Peer::NORMAL, pid);
}

void PeerManager::UpdatePeerRegStatus(const ISF isf, const int pid)
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
    //IntSet target_pieces = obj_pieceselect_->StartSelection(client_pid);
    MsgList req_msgs = obj_pieceselect_->StartSelection(client_pid);

    //MsgList req_msgs;
    //req_msgs = GetUndupDestReqMsgs(target_pieces, client_pid);
    //
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
        const int dest_pid = RandChooseElementInContainer(RSC::PIECESELECT, owners);
        const float pg_delay = client.get_neighbor_pgdelay(dest_pid);
        const float src_up_bw = client.get_bandwidth().uplink;
        msg = PieceMsg(client_pid, dest_pid, piece_no, pg_delay, src_up_bw);
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

void PeerManager::InitPeerRegInfo()
{
    const size_t NUM_PEER = g_btte_args.get_num_peer();

    if (g_peers_reg_info == nullptr)
    {
        g_peers_reg_info = new bool[NUM_PEER];
        if (g_peers_reg_info == nullptr)
           ExitError("Memory Allocation Error");

        for (size_t i = 0; i < NUM_PEER; i++)
            g_peers_reg_info[i] = false;
    }
}

void PeerManager::InitAboriginalPeers()
{
    InitSeeds();
    InitLeeches();
}

void PeerManager::CreateSwarm()
{
    InitPeerRegInfo();

    // Reserve spaces for vector of peers
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    g_peers.reserve(NUM_PEER);

    // allocate memroy for peers
    //AllocPeersSpace();

    InitAboriginalPeers();
}

void PeerManager::DeployPeersLevel()
{
    int count[g_kNumLevel];
    int exclude_set[g_kNumLevel];
    for (size_t i = 0; i < g_kNumLevel; i++)
    {
        count[i] = 0;
        exclude_set[i] = 0;
    }

    const size_t NUM_PEER = g_btte_args.get_num_peer();

    for (size_t p = 0; p < NUM_PEER; p++)
    {
        int level = RangeRandNumExceptEx<int>(RSC::PEER_LEVEL,
                                              g_kNumLevel,
                                              exclude_set);
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
}

void PeerManager::DeployClusterIDs()
{
    int count[g_kNumClusters];
    int exclude_set[g_kNumClusters];
    for (size_t i = 0; i < g_kNumClusters; i++)
    {
        count[i] = 0;
        exclude_set[i] = 0;
    }

    const size_t NUM_PEER = g_btte_args.get_num_peer();

    // assing a cid to each peer, and don't exceed
    // the volume (NUM_PEER / g_kNumClusters) of cluster
    for (size_t p = 0; p < NUM_PEER; p++)
    {
        int cid = RangeRandNumExceptEx<int>(RSC::CB_PEERSELECT,
                                            g_kNumClusters,
                                            exclude_set);
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
void PeerManager::InitSeeds()
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
        NewPeerData(Peer::LEECH, p, prob_leech);
    }
}
