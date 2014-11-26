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
    for (IntSetIter p_no = targets_set_.begin(); p_no != targets_set_.end(); p_no++, index++)
    {
        int counts = 0;
        for (auto& nei : neighbors)
        {
            if (HavePiece(nei.first, *p_no))
            {
                ++counts;
                piece_owner_set_[*p_no].insert(nei.first);
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
        counts_info_[index].piece_no = *p_no;
        counts_info_[index].counts = counts;
    }

    //neighbors = nullptr;
}

void RarestFirst::SortByPieceCounts(const int num_target)
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

bool RarestFirst::IsDupDest(const IntSet& dest_peers, const int nid)
{
    bool flag = false;
    for (const auto& it : dest_peers)
    {
        if (it == nid)
        {
            flag = true;
            break;
        }
    }
    return flag;
}

IntSet RarestFirst::GetFinalTargetPieces(const int num_target) const
{
    IntSet dup_count_pieces;
    IntSet final_target_pieces;
    for (size_t i = 1; i < num_target; ++i)
    {
        const int no = counts_info_[i].piece_no;
        const int count = counts_info_[i].counts;
        const int prev_count = counts_info_[i-1].counts;

        if (count == prev_count)
        {
            if (i == 1)
            {
                const int prev_no = counts_info_[i-1].piece_no;
                dup_count_pieces.insert(prev_no);
            }

            dup_count_pieces.insert(no);
        }
        else
        {
            if (i == 1)
            {
                const int prev_no = counts_info_[i-1].piece_no;
                final_target_pieces.insert(prev_no);
            }
            else if (i == num_target - 1)
            {
                final_target_pieces.insert(no);
            }

            if (dup_count_pieces.size() == 0)
            {
                final_target_pieces.insert(no);  // add current piece
            }
            else
            {
                const int rand_no = RandChooseSetElement(RSC::RF_PIECESELECT,
                                                         dup_count_pieces);
                final_target_pieces.insert(rand_no);
                dup_count_pieces.clear();
            }
        }
    }

    std::cout << "My final targets: \n";
    for (const int piece_no : final_target_pieces)
    {
        std::cout << piece_no << std::endl << std::endl;
    }

    return final_target_pieces;
}

MsgQueue RarestFirst::CreateReqMsgQ(IntSet const& final_target_pieces)
{
    MsgQueue req_msgs;
    IntSet dest_peers;

    size_t i = 0;
    for (const int piece_no : final_target_pieces)
    {
        ++i;
        auto& neighbors = g_peers.at(selector_pid_).get_neighbors();
        const int size = neighbors.size(); //for (int i = 0; i < size; i++)

        // If more than one neighbors having this piece,
        // then randomly choose one neighbor to request.
        IntSet const& owners = piece_owner_set_.at(piece_no);
        const int dest_pid = RandChooseSetElement(RSC::RF_PIECESELECT, owners);

        if (!IsDupDest(dest_peers, dest_pid))
        {
            PieceMsg msg;
            msg.src_pid = selector_pid_;
            msg.dest_pid = dest_pid;
            msg.piece_no = piece_no;
            msg.src_up_bw = g_peers.at(selector_pid_).get_bandwidth().uplink;
            req_msgs.push_back(msg);

            dest_peers.insert(dest_pid);
        }
    }
    std::cout << "Size of msg queue: " << req_msgs.size() << std::endl;

    return req_msgs;
}

void RarestFirst::RefreshInfo()
{
    delete [] counts_info_;
    counts_info_ = nullptr;  // important !!!
    targets_set_.clear();
}

MsgQueue RarestFirst::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    SetTargetPieces();

    const size_t num_target = targets_set_.size();

    CountNumPeerOwnPiece(num_target);

    SortByPieceCounts(num_target);

    // Create Req Msgs
    IntSet final_target_pieces = GetFinalTargetPieces(num_target);
    MsgQueue req_msgs = CreateReqMsgQ(final_target_pieces);

    for (const PieceMsg& msg : req_msgs)
    {
        Peer& client = g_peers.at(selector_pid_);
        Peer& peer = g_peers.at(msg.dest_pid);
        client.push_req_msg(msg);
        peer.push_recv_msg(msg);

        std::cout << "Piece Reqest Msg:" << std::endl;
        std::cout << "   src: " << msg.src_pid << std::endl;
        std::cout << "   dest: " << msg.dest_pid << std::endl;
        std::cout << "   wanted piece: " << msg.piece_no << std::endl;
        std::cout << "   src upload bandwidth: " << msg.src_up_bw << "\n\n";
    }
    RefreshInfo();

    return req_msgs;
}

}
