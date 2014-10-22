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

void RarestFirst::CountNumPeerOwnPiece(const int self_pid, const size_t num_targets)
{
    piece_counts_info_ = new PieceCounts[num_targets];

    const Neighbor *neighbors = g_peers[self_pid].neighbors;

    int index = 0;
    for (iSetIter it = targets_set_.begin(); it != targets_set_.end(); it++, index++)
    {
        int counts = 0;
        for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
        {
            const int nid = neighbors[n].id;
            bool is_get_piece = g_peers[nid].pieces[*it];
            if (is_get_piece) ++counts;
        }
        piece_counts_info_[index].piece_no = *it;
        piece_counts_info_[index].counts = counts;
    }

    neighbors = nullptr;
}

int RarestFirst::GetNumRarest(const int num_targets)
{
    int num_rarest = 0;

    for (int c = 0; c < num_targets; c++)
    {
        const int first = piece_counts_info_[c].counts;
        const int second = piece_counts_info_[c + 1].counts;
        if (first - second < 0)
        {
            num_rarest = c + 1;
            break;
        }
        else if (c == c - 2 &&
                 first - second == 0)
        {
            num_rarest = num_targets;
            break;
        }
    }

    std::cout << "Number of rarest piece: " << num_rarest << "\n";

    return num_rarest;
}

int RarestFirst::GetTargetPieceNo(const int num_rarest)
{
    int target_no = 0;

    if (num_rarest == 1)
    {
        target_no = piece_counts_info_[0].piece_no;
    }
    else
    {
        int index = Roll<int>(RSC::RF_PIECESELECT, 0, num_rarest - 1);
        target_no = piece_counts_info_[index].piece_no;
    }

    return target_no;
}

int RarestFirst::SelectTargetPiece(const int self_pid)
{
    CheckNeighbors(self_pid);

    SetTargetPieces(self_pid);

    const size_t num_targets = targets_set_.size();

    CountNumPeerOwnPiece(self_pid, num_targets);

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

    std::cout << "\nPiece Count Info (piece-no, counts) :\n";
    for (int i = 0; (size_t)i < num_targets; ++i)
    {
        std::cout << "(" << piece_counts_info_[i].piece_no << ",   "
                  << piece_counts_info_[i].counts << ")\n";
    }
    std::cout << std::endl;

    const int num_rarest = GetNumRarest(num_targets);

    const int target_piece_no = GetTargetPieceNo(num_rarest);

    // refresh info
    delete [] piece_counts_info_;
    piece_counts_info_ = nullptr;  // important !!!

    targets_set_.clear();

    return target_piece_no;
}

}
