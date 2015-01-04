#ifndef _RFP_PIECE_SELECTION_H
#define _RFP_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

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
