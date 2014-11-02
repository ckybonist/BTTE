#include <cstdlib>

#include "RF_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

RarestFirst::~RarestFirst()
{
    if (piece_counts_info_ != nullptr)
    {
        delete [] piece_counts_info_;
        piece_counts_info_ = nullptr;
    }
}

void RarestFirst::CountNumPeerOwnPiece(const size_t num_targets)
{
    piece_counts_info_ = new PieceCounts[num_targets];

    const Neighbor *neighbors = g_peers[selector_pid_].neighbors;

    int index = 0;
    for (IntSetIter it = targets_set_.begin(); it != targets_set_.end(); it++, index++)
    {
        int counts = 0;
        for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
        {
            if (neighbors[n].exist)  // ensure the neighbor is exist
            {
                const int nid = neighbors[n].id;
                bool is_get_piece = g_peers[nid].pieces[*it];
                if (is_get_piece) ++counts;
            }
        }
        piece_counts_info_[index].piece_no = *it;
        piece_counts_info_[index].counts = counts;
    }

    neighbors = nullptr;
}

void RarestFirst::SortByPieceCounts(const int num_targets)
{
    auto func_comp = [] (const void* a, const void* b) {
                           const PieceCounts* obj_a = (PieceCounts*)a;
                           const PieceCounts* obj_b = (PieceCounts*)b;
                           return obj_a->counts - obj_b->counts;
                        };

    // sort pieces counts info
    qsort(piece_counts_info_,
          num_targets,
          sizeof(PieceCounts),
          func_comp);
}

bool RarestFirst::IsDupReq(const IntSet& dest_peers, const int nid)
{
    bool flag = false;
    IntSetIter first = dest_peers.begin();
    IntSetIter last = dest_peers.end();
    for (IntSetIter it = first; it != last; ++it)
    {
        if (*it == nid)
        {
            flag = true;
            break;
        }
    }
    return flag;
}

PieceReqMsg RarestFirst::CreateReqMsg(const int target_piece_no, const bool is_last_piece)
{
    static IntSet dest_peers;
    PieceReqMsg msg;

    for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        const Neighbor &nei = g_peers[selector_pid_].neighbors[i];
        const int nid = nei.id;

        if (!nei.exist) continue;

        const bool piece_exist = g_peers[nid].pieces[target_piece_no];
        if (piece_exist && !IsDupReq(dest_peers, nid))
        {
            msg.src_pid = selector_pid_;
            msg.dest_pid = nid;
            msg.piece_no = target_piece_no;

            dest_peers.insert(nid);

            break;
        }
    }

    if (is_last_piece)
        dest_peers.clear();

    return msg;
}

//int RarestFirst::GetNumRarest(const int num_targets)
//{
//    int num_rarest = 0;
//
//    for (int c = 0; c < num_targets; c++)
//    {
//        const int first = piece_counts_info_[c].counts;
//        const int second = piece_counts_info_[c + 1].counts;
//        if (first - second < 0)
//        {
//            num_rarest = c + 1;
//            break;
//        }
//        else if (c == c - 2 &&
//                 first - second == 0)
//        {
//            num_rarest = num_targets;
//            break;
//        }
//    }
//
//    std::cout << "Number of rarest piece: " << num_rarest << "\n";
//
//    return num_rarest;
//}

//int RarestFirst::GetTargetPieceNo(const int num_rarest)
//{
//    int target_no = 0;
//
//    if (num_rarest == 1)
//    {
//        target_no = piece_counts_info_[0].piece_no;
//    }
//    else
//    {
//        int index = Roll<int>(RSC::RF_PIECESELECT, 0, num_rarest - 1);
//        target_no = piece_counts_info_[index].piece_no;
//    }
//
//    return target_no;
//}

void RarestFirst::RefreshInfo()
{
    delete [] piece_counts_info_;
    piece_counts_info_ = nullptr;  // important !!!
    targets_set_.clear();
}

PRMVec RarestFirst::StartSelection(const int self_pid)
{
    selector_pid_ = self_pid;

    CheckNeighbors();

    SetTargetPieces();

    const size_t num_targets = targets_set_.size();

    CountNumPeerOwnPiece(num_targets);

    SortByPieceCounts(num_targets);

    // debug info
    std::cout << "\nPiece Count Info (piece-no, counts) :\n";
    for (int i = 0; (size_t)i < num_targets; ++i)
    {
        std::cout << "(" << piece_counts_info_[i].piece_no << ",   "
                  << piece_counts_info_[i].counts << ")\n";
    }
    std::cout << std::endl;

    // Create req-msg list
    PRMVec req_msgs;
    for (int i = 0; (size_t)i < num_targets; ++i)
    {
        const int piece_no = piece_counts_info_[i].piece_no;
        const PieceReqMsg msg = CreateReqMsg(piece_no, ((size_t)i == num_targets - 1));
        if (msg.dest_pid != -1)
            req_msgs.push_back(msg);
    }

    RefreshInfo();

    return req_msgs;
}

}
