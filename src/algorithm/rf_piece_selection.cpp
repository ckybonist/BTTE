#include <cstdlib>

#include "rf_piece_selection.h"


using namespace uniformrand;


namespace btte_piece_selection
{

RarestFirst::~RarestFirst()
{
    if (count_info_ != nullptr)
    {
        delete [] count_info_;
        count_info_ = nullptr;
    }
}

void RarestFirst::CountNumPeerOwnPiece()
{
    //const Neighbor *neighbors = g_peers.at(selector_pid_).neighbors;
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();

    int index = 0;
    std::vector<POC> tmp_vec;
    IntSetIter begin = no_download_pieces_set_.begin();
    IntSetIter end = no_download_pieces_set_.end();

    for (IntSetIter p_no = begin; p_no != end; p_no++, index++)
    {
        int counts = 0;
        for (auto& nei : neighbors)
        {
            // NOTE: Not only check weather having this piece, but also
            // check the neighbor is in swarm
            if (HavePiece(nei.first, *p_no) &&
                    g_peers_reg_info[nei.first])
            {
                ++counts;
            }
        }

        if (counts > 0)
        {
            POC info = {*p_no, counts};
            tmp_vec.push_back(info);
        }
    }

    num_target_ = tmp_vec.size();
    count_info_ = new POC[num_target_];
    if (nullptr == count_info_)
        ExitError("Memory Allocation Error");

    std::copy(tmp_vec.begin(), tmp_vec.end(), count_info_);

    tmp_vec.clear();
}

void RarestFirst::SortByPieceCounts()
{
    auto cmp = [] (const void* l, const void* r) {
        const POC* myl = (POC*)l;
        const POC* myr = (POC*)r;
        return myl->count - myr->count;
    };

    // sort pieces counts info
    qsort(count_info_,
          num_target_,
          sizeof(POC),
          cmp);
}

IntSet RarestFirst::GetRarestPiecesSet() const
{
    IntSet target_pieces;
    IntSet dup_count_pieces;

    if (num_target_ == 1)  // only one
    {
        const int no = count_info_[0].piece_no;
        target_pieces.insert(no);
    }
    else
    {
        // Check peer-count of each piece iteratively, if appear same-peer-count situation,
        // then randomly choose one.
        // FIXME :  太冗長，應該有更好的處理邏輯
        for (size_t i = 1; i < num_target_; ++i)
        {
            const int cur_no = count_info_[i].piece_no;
            const int count = count_info_[i].count;
            const int prev_count = count_info_[i - 1].count;
            const int last_idx = num_target_ - 1;

            if (count == prev_count)
            {
                if (dup_count_pieces.empty())
                {
                    const int prev_no = count_info_[i - 1].piece_no;
                    dup_count_pieces.insert(prev_no);
                }

                dup_count_pieces.insert(cur_no);

                if (i == last_idx)
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
                    const int prev_no = count_info_[i - 1].piece_no;
                    target_pieces.insert(prev_no);
                }
                else if (!dup_count_pieces.empty())
                {
                    const int rand_no = RandChooseElementInSet(RSC::RF_PIECESELECT, dup_count_pieces);
                    target_pieces.insert(rand_no);
                    dup_count_pieces.clear();
                }
                else if (i == last_idx &&
                         dup_count_pieces.empty())
                {
                    target_pieces.insert(cur_no);
                }
            }
        }
    }

    return target_pieces;
}

void RarestFirst::RefreshInfo()
{
    delete [] count_info_;
    count_info_ = nullptr;
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

    RefreshInfo();

    return target_pieces;
}

}
