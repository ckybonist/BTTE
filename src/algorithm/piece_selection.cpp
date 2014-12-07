#include "piece_selection.h"

namespace btte_piece_selection
{

IPieceSelection::IPieceSelection()
{
}

IPieceSelection::~IPieceSelection()
{
}

void IPieceSelection::CheckNeighbors()
{
    //if (nullptr == g_peers.at(selector_pid_).neighbors)
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
    if (0 == neighbors.size())
    {
        std::cout << "\nYou don't have neighbors. Can't start Piece Selection\n";
        exit(0);
    }
}

void IPieceSelection::CollectNoDownloadPieces()
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();

    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        const Peer& selector = g_peers.at(selector_pid_);
        if (!selector.get_nth_piece_info(c))
            no_download_pieces_set_.insert(c);
    }
}

bool IPieceSelection::HavePiece(const int pid, const int piece_no) const
{
    bool have = g_peers.at(pid).get_nth_piece_info(piece_no);
    return have;
}

}
