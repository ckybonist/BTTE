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

void RarestFirst::CountNumPeerOwnPiece()
{
    //const Neighbor *neighbors = g_peers.at(selector_pid_).neighbors;
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();

    int index = 0;
    std::vector<PeerOwnCounts> tmp_vec;
    IntSetIter begin = no_download_pieces_set_.begin();
    IntSetIter end = no_download_pieces_set_.end();

    for (IntSetIter p_no = begin; p_no != end; p_no++, index++)
    {
        int counts = 0;
        std::cout << "\nOwners of piece #" << *p_no << std::endl;
        for (auto& nei : neighbors)
        {
            // NOTE: Not only check weather having this piece, but also
            // check the neighbor is in swarm
            if (HavePiece(nei.first, *p_no) &&
                    g_peers_reg_info[nei.first])
            {
                std::cout << nei.first << std::endl;
                ++counts;
            }
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
        if (counts > 0)
        {
            PeerOwnCounts info = {*p_no, counts};
            tmp_vec.push_back(info);
        }
    }

    num_target_ = tmp_vec.size();
    counts_info_ = new PeerOwnCounts[num_target_];
    if (nullptr == counts_info_)
        ExitError("Memory Allocation Error");

    std::copy(tmp_vec.begin(), tmp_vec.end(), counts_info_);

    tmp_vec.clear();
    //neighbors = nullptr;
}

void RarestFirst::SortByPieceCounts()
{
    auto cmp = [] (const void* l, const void* r) {
        const PeerOwnCounts* myl = (PeerOwnCounts*)l;
        const PeerOwnCounts* myr = (PeerOwnCounts*)r;
        return myl->counts - myr->counts;
    };

    // sort pieces counts info
    qsort(counts_info_,
          num_target_,
          sizeof(PeerOwnCounts),
          cmp);

    // debug info
    //std::cout << "\nPiece Count Info (piece-no, counts) :\n";
    //for (size_t i = 0; i < num_target_; ++i)
    //{
    //    std::cout << "(" << counts_info_[i].piece_no << ",   "
    //              << counts_info_[i].counts << ")\n";
    //}
    //std::cout << std::endl;
}

IntSet RarestFirst::GetRarestPiecesSet() const
{
    IntSet target_pieces;
    IntSet dup_count_pieces;

    if (num_target_ == 1)
    {
        const int no = counts_info_[0].piece_no;
        target_pieces.insert(no);
    }
    else
    {
        // Check peer-count of each piece iteratively, if appear same-peer-count situation,
        // then randomly choose one.
        // FIXME : BUG, 太冗長
        for (size_t i = 1; i < num_target_; ++i)
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

                if (num_target_ == 2)
                {
                    const int rand_no = RandChooseElementInSet(RSC::RF_PIECESELECT, dup_count_pieces);
                    target_pieces.insert(rand_no);
                    dup_count_pieces.clear();
                }
            }
            else
            {
                if (i == 1)
                {
                    const int prev_no = counts_info_[i - 1].piece_no;
                    target_pieces.insert(prev_no);
                }
                else if (i == num_target_ - 1 ||
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

    CountNumPeerOwnPiece();

    SortByPieceCounts();

    IntSet target_pieces = GetRarestPiecesSet();

    std::cout << "\nResult of Rarest First Piece Selection:\n";
    for (const int no : target_pieces)
        std::cout << no << std::endl;
    std::cout << std::endl;

    RefreshInfo();

    return target_pieces;
}

}
