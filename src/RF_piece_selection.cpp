#include <cstdlib>
//#include <algorithm>

#include "RF_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

RarestFirst::~RarestFirst()
{
    if (counts_info_ != nullptr)
    {
        delete [] counts_info_;
        counts_info_ = nullptr;
    }
}

void RarestFirst::CountNumPeerOwnPiece(const size_t num_targets)
{
    counts_info_ = new PeerOwnCounts[num_targets];

    //const Neighbor *neighbors = g_peers.at(selector_pid_).neighbors;
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();

    int index = 0;
    for (IntSetIter p_no = targets_set_.begin(); p_no != targets_set_.end(); p_no++, index++)
    {
        int counts = 0;
        for (auto& nei : neighbors)
        {
            if (HavePiece(nei.first, *p_no))
            {
                ++counts;
                piece_owner_set_[*p_no].insert(nei.first);
            }
        }
        //for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
        //{
        //    if (neighbors[n].exist)  // ensure the neighbor is exist
        //    {
        //        const int nid = neighbors[n].id;
        //        bool is_get_piece = g_peers.at(nid).pieces[*it];
        //        if (is_get_piece) ++counts;
        //    }
        //}
        counts_info_[index].piece_no = *p_no;
        counts_info_[index].counts = counts;
    }

    //neighbors = nullptr;
}

void RarestFirst::SortByPieceCounts(const int num_targets)
{
    auto cmp = [] (const void* l, const void* r) {
                      const PeerOwnCounts* myl = (PeerOwnCounts*)l;
                      const PeerOwnCounts* myr = (PeerOwnCounts*)r;
                      return myl->counts - myr->counts;
                  };

    // sort pieces counts info
    qsort(counts_info_,
          num_targets,
          sizeof(PeerOwnCounts),
          cmp);
}

bool RarestFirst::IsDupDest(const IntSet& dest_peers, const int nid)
{
    bool flag = false;
    IntSetIter begin = dest_peers.begin();
    IntSetIter end = dest_peers.end();
    for (IntSetIter it = begin; it != end; ++it)
    {
        if (*it == nid)
        {
            flag = true;
            break;
        }
    }
    return flag;
}

//PieceMsg RarestFirst::CreateReqMsg(const int target_piece_no, const bool is_last_piece)
PieceMsg RarestFirst::CreateReqMsg(const int target_piece_no, IntSet& dest_peers)
{
    //static IntSet dest_peers;  // record peer-ids which existed in request msgs
    PieceMsg msg;

    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
    const int size = neighbors.size();
    //for (int i = 0; i < size; i++)
    //{
    //if (which_have_set.size() == 0) continue;

    // If more than one neighbors having this piece,
    // then randomly choose one neighbor to request.
    IntSet const& owners = piece_owner_set_.at(target_piece_no);
    const int rand_pos = Rand(RSC::RF_PIECESELECT) % owners.size();
    auto it = owners.begin();
    for (int i = 0; i < rand_pos; ++i, ++it);
    const int dest = *it;

    if (!IsDupDest(dest_peers, dest))
    {
        msg.src_pid = selector_pid_;
        msg.dest_pid = dest;
        msg.piece_no = target_piece_no;
        msg.src_up_bw = g_peers.at(selector_pid_).get_bandwidth().uplink;
        dest_peers.insert(dest);
        //break;
    }
    //}

    //for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    //{
    //    auto nei = g_peers.at(selector_pid_).neighbors[i];

    //    if (!nei.exist) continue;

    //    if (IsDownloadable(nei, target_piece_no) &&
    //        !IsDupDest(dest_peers, nei.id))
    //    //if (!IsDupReq(dest_peers, nid))
    //    {
    //        msg.src_pid = selector_pid_;
    //        msg.dest_pid = nei.id;
    //        msg.piece_no = target_piece_no;
    //        dest_peers.insert(nei.id);
    //        break;
    //    }
    //}
    //if (is_last_piece) dest_peers.clear();

    return msg;
}

void RarestFirst::RefreshInfo()
{
    delete [] counts_info_;
    counts_info_ = nullptr;  // important !!!
    targets_set_.clear();
}

MsgQueue RarestFirst::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    SetTargetPieces();

    const size_t num_targets = targets_set_.size();

    CountNumPeerOwnPiece(num_targets);

    SortByPieceCounts(num_targets);

    // debug info
    std::cout << "\nPiece Count Info (piece-no, counts) :\n";
    for (size_t i = 0; i < num_targets; ++i)
    {
        std::cout << "(" << counts_info_[i].piece_no << ",   "
                  << counts_info_[i].counts << ")\n";
    }
    std::cout << std::endl;

    // Create req-msg list
    int num_same_counts = 0;
    int total_piece_counts = 0;
    IntSet pieces_same_counts;
    MsgQueue req_msgs;

    for (size_t i = 0; i < num_targets; ++i)
    {
        IntSet dest_peers;
        const int piece_no = counts_info_[i].piece_no;
        const int piece_counts = counts_info_[i].counts;

        //if (msg.dest_pid != -1)
        //    req_msgs.push_back(msg);
        //const PieceMsg msg = CreateReqMsg(piece_no, ((size_t)i == num_targets - 1));
        PieceMsg const& msg = CreateReqMsg(piece_no, dest_peers);
        req_msgs.push_back(msg);

        if (i == num_targets - 1)
            dest_peers.clear();
    }

    std::cout << "\nhaha\n";

    for (const PieceMsg& msg : req_msgs)
    {
        Peer& client = g_peers.at(selector_pid_);      // self peer
        Peer& peer = g_peers.at(msg.dest_pid);         // other peer
        //client.pieces_on_req.insert(msg.piece_no);
        client.push_req_msg(msg);
        peer.push_recv_msg(msg);

        std::cout << "Sending piece-req msg from peer #" << msg.src_pid << " to peer #"
                  << msg.dest_pid << std::endl;
        std::cout << "Wanted piece: " << msg.piece_no << "\n\n";
    }
    RefreshInfo();

    return req_msgs;
}

}
