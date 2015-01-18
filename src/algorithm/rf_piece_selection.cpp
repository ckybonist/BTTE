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

int RarestFirst::CountNumPeerOwnPiece(const int piece_no)
{
    auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
    int counts = 0;
    for (auto& nei : neighbors)
    {
        // NOTE: Not only check weather neighbor own this piece, but also
        // check the it's in swarm
        if (HavePiece(nei.first, piece_no) &&
            g_peers_reg_info[nei.first]) { ++counts; }
    }

    return counts;
}

void RarestFirst::SetCountInfo()
{
    int index = 0;
    std::vector<POC> tmp_vec;
    std::vector<int>::iterator begin = candidates_.begin();
    std::vector<int>::iterator end = candidates_.end();

    for (auto p_no = begin; p_no != end; p_no++, index++)
    {
        const int counts = CountNumPeerOwnPiece(*p_no);
        if (counts > 0)
        {
            POC info = {*p_no, counts};
            tmp_vec.push_back(info);
        }
    }

    count_info_ = new POC[candidates_.size()];
    if (nullptr == count_info_)
        ExitError("Memory Allocation Error");
    std::copy(tmp_vec.begin(), tmp_vec.end(), count_info_);
    tmp_vec.clear();
}

void RarestFirst::ShuffleCountInfo(const RSC rsc, POC *arr)
{
    if (arr == nullptr)
    {
        ExitError("\nPassing nullptr as an array\n");
    }

    const int N = candidates_.size();
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
    auto cmp = [] (const void* l, const void* r) {
        const POC* myl = (POC*)l;
        const POC* myr = (POC*)r;
        return myl->count - myr->count;
    };

    // sort pieces counts info
    qsort(count_info_,
          candidates_.size(),
          sizeof(POC),
          cmp);
}

void RarestFirst::RefreshInfo()
{
    delete [] count_info_;
    count_info_ = nullptr;
    candidates_.clear();
}

void RarestFirst::ProcessCountInfo()
{
    SetCountInfo();
    ShuffleCountInfo(RSC::PIECESELECT, count_info_);
    SortByPieceCounts();
}

MsgList RarestFirst::GenrMsgs()
{
    std::vector<int> rf_candidates;
    for(int i = 0; i < candidates_.size(); i++)
        rf_candidates.push_back(count_info_[i].piece_no);

    MsgList req_msgs = GetUndupDestReqMsgs(rf_candidates, selector_pid_);
    return req_msgs;
}

MsgList RarestFirst::StartSelection(const int client_pid)
{
    MsgList req_msgs;
    selector_pid_ = client_pid;

    CheckNeighborExist();
    SetCandidates();

    if (candidates_.size() > 0)
    {
        ProcessCountInfo();
        req_msgs = GenrMsgs();
    }

    //Debug(req_msgs, rf_candidates);

    RefreshInfo();

    return req_msgs;
}

}
