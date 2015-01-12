#include <fstream>
#include "../args.h"
#include "rfp_piece_selection.h"

using namespace btte_uniformrand;


namespace btte_piece_selection
{

void RandomFirstPiece::RefreshInfo()
{
    candidates_.clear();
}

MsgList RandomFirstPiece::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighborExist();

    SetCandidates();

    MsgList req_msgs = GetUndupDestReqMsgs(candidates_, selector_pid_);

    Debug(req_msgs, std::vector<int>());

    RefreshInfo();

    return req_msgs;
}

}
