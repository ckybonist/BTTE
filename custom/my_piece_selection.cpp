#include "my_piece_selection.h"


using namespace btte_uniformrand;


MyPieceSelection::~MyPieceSelection()
{

}

IntSet MyPieceSelection::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    /*
     * Check selector have neighbors
     * */
    CheckNeighbors();

    /* Collect pieces which have not download yet
     * into candidates_
     * */
    SetCandidates();

    IntSet target_pieces;

    /*
     * Use your algorithm to select pieces in candidates_
     * and insert to specified pieces into "target_pieces"
     * * * * */

    return target_pieces;
}
