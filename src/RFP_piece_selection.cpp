#include "RFP_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

PieceReqMsg RandomFirstPiece::CreateReqMsg(const int target_piece_no)
{
    PieceReqMsg msg;

    for(int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        const Neighbor &nei = g_peers[selector_pid_].neighbors[i];
        if (!nei.exist) continue;

        const int nid = nei.id;
        const bool piece_exist = g_peers[nid].pieces[target_piece_no];
        if (piece_exist)
        {
            msg.src_pid = selector_pid_;
            msg.dest_pid = nid;
            msg.piece_no = target_piece_no;
        }
    }

    return msg;
}

void RandomFirstPiece::RefreshInfo()
{
    targets_set_.clear();
}

PRMVec RandomFirstPiece::StartSelection(const int self_pid)
{
    selector_pid_ = self_pid;

    CheckNeighbors();

    SetTargetPieces();

    const size_t num_targets = targets_set_.size();

    // Randomly select a piece no from set
    IntSetIter piece_no = targets_set_.begin();
    int rand_offset = Rand(RSC::RFP_PIECESELECT) % num_targets;
    for(; rand_offset != 0; --rand_offset) ++piece_no;

    std::vector<PieceReqMsg> req_msgs;
    const PieceReqMsg msg = CreateReqMsg(*piece_no);
    if (msg.dest_pid != -1)
        req_msgs.push_back(msg);

    RefreshInfo();

    return req_msgs;
}

}
