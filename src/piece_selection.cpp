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

    // check the piece has requested but not get it
    bool still_download = false;
    auto selector = g_peers.at(selector_pid_);
    auto begin = selector.send_msgs.begin();
    auto end = selector.send_msgs.end();
    for (auto it = begin; it != end; ++it)
    {
        if ((*it).piece_no == target_piece_no)
        {
            still_download = true;
            break;
        }
    }

    // Check the neighbor not choke the selector and the selector is interested with neighbor
    // Finally, this request is not send ever
    const bool allow_download = conn_states.am_interested &&
                                    !conn_states.peer_choking &&
                                    !still_download;

    return allow_download;
}

}
