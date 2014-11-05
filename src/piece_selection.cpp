#include "piece_selection.h"

namespace btte_piece_selection
{

IPieceSelect::IPieceSelect(Args args)
{
    args_ = args;
}

IPieceSelect::~IPieceSelect()
{

}

void IPieceSelect::CheckNeighbors()
{
    if (nullptr == g_peers.at(selector_pid_).neighbors)
    {
        std::cout << "\nYou don't have neighbors. So there is no way to select a target piece\n";
        exit(0);
    }
}

void IPieceSelect::SetTargetPieces()
{
    for (int c = 0; (size_t)c < args_.NUM_PIECE; c++)
    {
        auto selector = g_peers.at(selector_pid_);
        if (!selector.pieces[c])
            targets_set_.insert(c);
    }
}

bool IPieceSelect::IsDownloadable(Neighbor& nei, const int target_piece_no) const
{
    const int nid = nei.id;
    auto conn_states = nei.conn_states;
    const bool have_piece = g_peers.at(nid).pieces[target_piece_no];
    conn_states.am_interested = (have_piece && !conn_states.am_choking) ? true : false;

    // Check the neighbor can share with selector and selector is interested with neighbor
    const bool allow_download = conn_states.am_interested && !conn_states.peer_choking;

    return allow_download;
}

}
