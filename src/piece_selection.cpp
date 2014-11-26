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
    //if (nullptr == g_peers.at(selector_pid_).neighbors)
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
    if (0 == neighbors.size())
    {
        std::cout << "\nYou don't have neighbors. So there is no way to select a target piece\n";
        exit(0);
    }
}

void IPieceSelect::SetTargetPieces()
{
    for (size_t c = 0; c < args_.NUM_PIECE; c++)
    {
        const Peer& selector = g_peers.at(selector_pid_);
        if (!selector.get_nth_piece(c))
            targets_set_.insert(c);
    }
}

bool IPieceSelect::HavePiece(const int pid, const int piece_no) const
{
    return g_peers.at(pid).get_nth_piece(piece_no);
}

}
