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
    if (nullptr == g_peers[selector_pid_].neighbors)
    {
        std::cout << "\nYou don't have neighbors. So there is no way to select a target piece\n";
        exit(0);
    }
}

void IPieceSelect::SetTargetPieces()
{
    for (int c = 0; (size_t)c < args_.NUM_PIECE; c++)
    {
        if (!g_peers[selector_pid_].pieces[c])
            targets_set_.insert(c);
    }
}

}
