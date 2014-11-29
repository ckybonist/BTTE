#ifndef _RFP_PIECE_SELECTION_H
#define _RFP_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

class RandomFirstPiece : public IPieceSelect
{
  public:
    RandomFirstPiece() : IPieceSelect() {};

  private:
    IntSet StartSelection(const int client_pid) override;
    IntSet GetTargetPieces(const size_t num_target) const;
    void RefreshInfo();
};

}

#endif // for #ifndef _RFP_PIECE_SELECTION_H
