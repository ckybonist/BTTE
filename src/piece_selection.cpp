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

void IPieceSelect::CheckNeighbors(const int self_pid)
{
    if (nullptr == g_peers[self_pid].neighbors)
    {
        std::cout << "\nYou don't have neighbors. So there is no way to select a target piece\n";
        exit(0);
    }
}

void IPieceSelect::SetTargetPieces(const int self_pid)
{
    for (int c = 0; (size_t)c < args_.NUM_PIECE; c++)
    {
        if (!g_peers[self_pid].pieces[c])
            targets_set_.insert(c);
    }
}

}
