#include "my_piece_selection.h"


using namespace btte_uniformrand;


MyPieceSelection::~MyPieceSelection()
{

}

MsgList MyPieceSelection::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    /*
     * Check selector have neighbors
     * */
    CheckNeighborExist();

    /* Collect pieces which have not download yet
     * into candidates_
     * */
    SetCandidates();

    MsgList req_msgs;

    /*
     * Use your algorithm to select pieces in candidates_
     * and insert to specified pieces into "target_pieces"
     * * * * */

    return req_msgs;
}
