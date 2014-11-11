#include "RFP_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

PieceMsg RandomFirstPiece::CreateReqMsg(const int target_piece_no)
{
    PieceMsg msg;

    for(int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        auto nei = g_peers[selector_pid_].neighbors[i];

        if (!nei.connected) continue;

        if (IsDownloadable(nei, target_piece_no))
        {
            msg.src_pid = selector_pid_;
            msg.dest_pid = nei.id;
            msg.piece_no = target_piece_no;
        }
    }

    return msg;
}

void RandomFirstPiece::RefreshInfo()
{
    targets_set_.clear();
}

PMList RandomFirstPiece::StartSelection(const int self_pid)
{
    selector_pid_ = self_pid;

    CheckNeighbors();

    SetTargetPieces();

    const size_t num_targets = targets_set_.size();

    // Randomly select a piece no from set
    IntSetIter piece_no = targets_set_.begin();
    int rand_offset = Rand(RSC::RFP_PIECESELECT) % num_targets;
    for(; rand_offset != 0; --rand_offset) ++piece_no;

    std::list<PieceMsg> req_msgs;
    const PieceMsg msg = CreateReqMsg(*piece_no);
    if (msg.dest_pid != -1)
        req_msgs.push_back(msg);

    RefreshInfo();

    return req_msgs;
}

}
