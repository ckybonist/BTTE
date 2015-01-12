#include <cstdlib>
#include <fstream>
#include "rf_piece_selection.h"


using namespace btte_uniformrand;


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
    std::vector<int>::iterator begin = candidates_.begin();
    std::vector<int>::iterator end = candidates_.end();

    for (auto p_no = begin; p_no != end; p_no++, index++)
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

void RarestFirst::ShuffleCountInfo(const RSC rsc, POC *arr, const size_t N)
{
    if (arr == nullptr)
    {
        ExitError("\nPassing nullptr as an array\n");
    }

    if (N < 1)
    {
        ExitError("\nSize must greater than 1\n");
    }

    for (std::size_t i = 0; i < N; i++)
    {
        std::size_t idx = Roll<int>(rsc, 0, N - 1);
        POC temp = arr[idx];
        arr[idx] = arr[i];
        arr[i] = temp;
    }
}

void RarestFirst::SortByPieceCounts()
{
    auto cmp = [] (const void* l, const void* r) { const POC* myl = (POC*)l;
        const POC* myr = (POC*)r;
        return myl->count - myr->count;
    };

    // sort pieces counts info
    qsort(count_info_,
          num_target_,
          sizeof(POC),
          cmp);
}

void RarestFirst::RefreshInfo()
{
    delete [] count_info_;
    count_info_ = nullptr;
    candidates_.clear();
}

MsgList RarestFirst::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighborExist();

    SetCandidates();

    CountNumPeerOwnPiece();

    ShuffleCountInfo(RSC::PIECESELECT, count_info_, num_target_);

    SortByPieceCounts();

    std::vector<int> rf_candidates;
    for(int i = 0; i < num_target_; i++)
        rf_candidates.push_back(count_info_[i].piece_no);

    MsgList req_msgs = GetUndupDestReqMsgs(rf_candidates, selector_pid_);

    //Debug(req_msgs, rf_candidates);

    RefreshInfo();

    return req_msgs;
}

}
