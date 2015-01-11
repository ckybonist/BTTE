#ifndef _RFP_PIECE_SELECTION_H
#define _RFP_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

/*
 * Current Implementaion is not same as Random First Piece,
 * our rand function is use in choosing owner of each piece
 * (see peer_manager.cpp).
 *
 * */
class RandomFirstPiece : public IPieceSelection
{
  public:
    RandomFirstPiece() : IPieceSelection() {};

  private:
    IntSet StartSelection(const int client_pid) override;
    IntSet GetRandomPieceSet();
    void RefreshInfo();
};

}

#endif // for #ifndef _RFP_PIECE_SELECTION_H
