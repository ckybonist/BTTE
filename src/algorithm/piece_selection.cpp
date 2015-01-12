#include <fstream>

#include "../random.h"
#include "general_algo.h"
#include "piece_selection.h"


using namespace btte_uniformrand;


namespace btte_piece_selection
{

IPieceSelection::IPieceSelection()
{

}

IPieceSelection::~IPieceSelection()
{

}

void IPieceSelection::CheckNeighborExist()
{
    //if (nullptr == g_peers.at(selector_pid_).neighbors)
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
    if (0 == neighbors.size())
    {
        std::cout << "\nYou don't have neighbors.\n";
        exit(0);
    }
}

void IPieceSelection::SetCandidates()
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();

    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        const Peer& selector = g_peers.at(selector_pid_);
        bool flag = false;
        if (!selector.get_nth_piece_info(c))
        {
            // checking someone own this piece
            for (auto const& it : neighbors)
                if (HavePiece(it.first, c))
                {
                    //candidates_.insert(c);
                    candidates_.push_back(c);
                    break;
                }
        }
    }
}

bool IPieceSelection::HavePiece(const int pid, const int piece_no) const
{
    bool have = g_peers.at(pid).get_nth_piece_info(piece_no);
    return have;
}

void IPieceSelection::CheckNeighborAlive()
{
    Peer& client = g_peers.at(selector_pid_);
    for (auto const& nei : client.get_neighbors())
    {
        const int pid = nei.first;
        if (!g_peers_reg_info[pid])  // peer is leave
        {
            client.remove_neighbor(pid);
        }
    }
}

IntSet IPieceSelection::GetPieceOwners(const int piece_no, const int client_pid)
{
    IntSet owners;
    Peer& client = g_peers.at(client_pid);
    CheckNeighborAlive();

    /*
     * Not only check weather having this piece, but also
     * check the neighbor is in swarm
     *
     * if neighbor is leave, then remove it from client's peer list
     *
     * * */
    for (auto const& nei : client.get_neighbors())
    {
        const int pid = nei.first;
        if (HavePiece(pid, piece_no))
            owners.insert(pid);
    }

    return owners;
}


//std::vector<int> IPieceSelection::GetNonReqPieceOwners(const int no, const int client_pid)
IntSet IPieceSelection::GetNonReqPieceOwners(const int no, const int client_pid)
{
    // Collect peers which on request to a set
    MsgList send_msg_buf = g_peers.at(client_pid).get_send_msg_buf();
    IntSet on_req_peers;
    for (auto const& msg : send_msg_buf)
        on_req_peers.insert(msg.dest_pid);

    // 從這個 piece 的持有者中再去除掉之前有送過要求但還沒拿到 piece 的持有者，
    // 也就是不要對同一個 neighbor 送超過一個要求。
    IntSet orig_owners = GetPieceOwners(no, client_pid);
    IntVec tmp(orig_owners.size(), -1);
    btte_set_diff(orig_owners.begin(), orig_owners.end(),
                  on_req_peers.begin(), on_req_peers.end(),
                  tmp.begin());

    // Remove invalid result (equalt to the value which use to init vector)
    IntVec::iterator it = tmp.begin();
    for (; it != tmp.end(); ++it)
    {
       if (*it == -1) break;
    }
    tmp.erase(it, tmp.end());

    IntSet result;
    for (const int pid : tmp) result.insert(pid);

    return result;
}

//std::map<int, std::vector<int>>
std::map<int, IntSet>
IPieceSelection::GetPieceOwnersMap(IntSet const& target_pieces, const int client_pid)
{
    //std::map<int, IntVec> piece_owner_map;
    std::map<int, IntSet> piece_owner_map;

    // 蒐集所有 piece 的持有者（已過濾）
    for (const int no : target_pieces)
        piece_owner_map[no] = GetNonReqPieceOwners(no, client_pid);

    return piece_owner_map;
}

bool IPieceSelection::IsDupDest(const IntSet& dest_peers, const int nid)
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

PieceMsg IPieceSelection::MakeMsg(const int piece_no, const int dest_pid)
{
    Peer const& client = g_peers.at(selector_pid_);
    const float pg_delay = client.get_neighbor_pgdelay(dest_pid);
    const float src_up_bw = client.get_bandwidth().uplink;

    PieceMsg msg(selector_pid_, dest_pid, piece_no, pg_delay, src_up_bw);
    return msg;
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
//MsgList IPieceSelection::GetUndupDestReqMsgs(IntSet const& target_pieces, const int client_pid)
MsgList IPieceSelection::GetUndupDestReqMsgs(std::vector<int> const& target_pieces, const int client_pid)
{
    Peer& client = g_peers.at(client_pid);
    //I2IVecMap piece_owners_map = GetPieceOwnersMap(target_pieces, client_pid);
    //std::map<int, IntSet> piece_owners_map = GetPieceOwnersMap(target_pieces, client_pid);
    std::vector<int> tmp_target = target_pieces;

    MsgList req_msgs;
    IntSet owners ;
    std::vector<int>::iterator begin = tmp_target.begin();
    for (const int piece_no : target_pieces)
    {
        IntSet owners = GetNonReqPieceOwners(piece_no, client_pid);

        if (owners.size() != 0)
        {
            // If more than one neighbor have this piece,
            // then randomly choose one neighbor to request.
            //const int dest_pid = RandChooseElementInContainer(RSC::PIECESELECT, owners);
            const int dest_pid = RandChooseElementInSet(RSC::PIECESELECT, owners);
            PieceMsg msg = MakeMsg(piece_no, dest_pid);
            req_msgs.push_back(msg);
            client.push_send_msg(msg);
        }
    }

    return req_msgs;
}

void IPieceSelection::Debug(MsgList const& req_msgs, std::vector<int> rf_candidates)
{
    std::ofstream ofs;
    std::string prefix = " ";
    switch (g_btte_args.get_type_pieceselect())
    {
        case 0:
            prefix = "rand_";
            break;
        case 1:
            prefix = "rarefst_";
            break;
        case 2:
            prefix = "user_";
            break;
        default:
            ExitError("wrong algo ID");
        break;
    }
    ofs.open(prefix + "piecesel_log.txt", std::fstream::app);


    ofs << "client 尚未取得 pieces :\n";
    for (const int no : candidates_) ofs << no << ' ';
    ofs << "\n\n";

    auto const& neighbors = g_peers.at(selector_pid_).get_neighbors();
    ofs << "client 的 neighbors :\n";
    for (auto const& it : neighbors)
    {
        const int pid = it.first;
        if (g_peers_reg_info[pid])
            ofs << pid << ' ';
    }
    ofs << "\n\n";

    ofs << "正在要求的 neighbors :\n";
    MsgList send_msg_buf = g_peers.at(selector_pid_).get_send_msg_buf();
    for (PieceMsg const& msg : send_msg_buf)
    {
        bool flag = true;
        for (PieceMsg const& req : req_msgs)
        {
            if (req.dest_pid == msg.dest_pid) flag = false;
        }
        if (flag) ofs << msg.dest_pid << ' ';
    }
    ofs << "\n\n";

    ofs << "每個 piece 的擁有者(neigbor)：\n";
    ofs << "<piece>  <owners>\n";
    if (g_btte_args.get_type_pieceselect() == 1)
    {
        for (const int no : rf_candidates)
        {
            ofs << no << " : { ";
            // list oweners
            for (auto const& it : neighbors)
            {
                if (g_peers.at(it.first).get_nth_piece_info(no))
                    ofs << it.first << ' ';
            }
            ofs << "}\n";
        }
    }
    else
    {
        for (const int no : candidates_)
        {
            ofs << no << " : { ";
            // list oweners
            for (auto const& it : neighbors)
            {
                if (g_peers.at(it.first).get_nth_piece_info(no))
                    ofs << it.first << ' ';
            }
            ofs << "}\n";
        }
    }

    ofs << "\n\n";

    ofs << "對於每個 piece，亂數挑選擁有者，如果此擁有者正在要求則不算：\n";
    ofs << "<piece>  <owner>\n";
    for (PieceMsg const& msg : req_msgs)
    {
        ofs << msg.piece_no << ' ' << msg.dest_pid << std::endl;
    }
    ofs << "-------------------------------\n\n";
    ofs.close();
}

}
