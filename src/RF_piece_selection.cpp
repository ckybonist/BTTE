#include <cstdlib>
#include "RF_piece_selection.h"


using namespace uniformrand;


namespace btte_piece_selection
{

RarestFirst::~RarestFirst()
{
    if (counts_info_ != nullptr)
    {
        delete [] counts_info_;
        counts_info_ = nullptr;
    }
}

void RarestFirst::CountNumPeerOwnPiece(const size_t num_target)
{
    counts_info_ = new PeerOwnCounts[num_target];

    //const Neighbor *neighbors = g_peers.at(selector_pid_).neighbors;
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();

    int index = 0;
    IntSetIter begin = no_download_pieces_set_.begin();
    IntSetIter end = no_download_pieces_set_.end();
    for (IntSetIter p_no = begin; p_no != end; p_no++, index++)
    {
        int counts = 0;
        for (auto& nei : neighbors)
        {
            if (HavePiece(nei.first, *p_no)) ++counts;
        }
        //for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
        //{
        //    if (neighbors[n].exist)  // ensure the neighbor is exist
        //    {
        //        const int nid = neighbors[n].id;
        //        bool is_get_piece = g_peers.at(nid).pieces[*it];
        //        if (is_get_piece) ++counts;
        //    }
        //}
        counts_info_[index].piece_no = *p_no;
        counts_info_[index].counts = counts;
    }

    //neighbors = nullptr;
}

void RarestFirst::SortByPieceCounts(const size_t num_target)
{
    auto cmp = [] (const void* l, const void* r) {
        const PeerOwnCounts* myl = (PeerOwnCounts*)l;
        const PeerOwnCounts* myr = (PeerOwnCounts*)r;
        return myl->counts - myr->counts;
    };

    // sort pieces counts info
    qsort(counts_info_,
          num_target,
          sizeof(PeerOwnCounts),
          cmp);

    // debug info
    std::cout << "\nPiece Count Info (piece-no, counts) :\n";
    for (size_t i = 0; i < num_target; ++i)
    {
        std::cout << "(" << counts_info_[i].piece_no << ",   "
                  << counts_info_[i].counts << ")\n";
    }
    std::cout << std::endl;
}

IntSet RarestFirst::GetRarestPiecesSet(const size_t num_target) const
{
    IntSet target_pieces;
    IntSet dup_count_pieces;

    // Check peer-count of each piece iteratively, if appear same-peer-count situation,
    // then randomly choose one.
    for (size_t i = 1; i < num_target; ++i)
    {
        const int no = counts_info_[i].piece_no;
        const int count = counts_info_[i].counts;
        const int prev_count = counts_info_[i - 1].counts;

        if (count == prev_count)
        {
            if (i == 1)
            {
                const int prev_no = counts_info_[i - 1].piece_no;
                dup_count_pieces.insert(prev_no);
            }

            dup_count_pieces.insert(no);
        }
        else
        {
            if (i == 1)
            {
                const int prev_no = counts_info_[i - 1].piece_no;
                target_pieces.insert(prev_no);
            }
            else if (i == num_target - 1 ||
                     dup_count_pieces.size() == 0)
            {
                target_pieces.insert(no);
            }
            else
            {
                const int rand_no = RandChooseElementInSet(RSC::RF_PIECESELECT, dup_count_pieces);
                target_pieces.insert(rand_no);
                dup_count_pieces.clear();
            }
        }
    }

    std::cout << "My final targets: \n";
    for (const int piece_no : target_pieces)
    {
        std::cout << piece_no << std::endl << std::endl;
    }

    return target_pieces;
}

void RarestFirst::RefreshInfo()
{
    delete [] counts_info_;
    counts_info_ = nullptr;
    no_download_pieces_set_.clear();
}

IntSet RarestFirst::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    CollectNoDownloadPieces();

    const size_t num_target = no_download_pieces_set_.size();

    CountNumPeerOwnPiece(num_target);

    SortByPieceCounts(num_target);

    IntSet target_pieces = GetRarestPiecesSet(num_target);

    RefreshInfo();

    return target_pieces;
}

}
