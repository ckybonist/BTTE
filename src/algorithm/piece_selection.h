#ifndef _PIECE_SELECTION_H
#define _PIECE_SELECTION_H

#include <iostream>
#include <vector>
#include <deque>
#include <set>

#include "../args.h"
#include "../random.h"
#include "../peer.h"
#include "../neighbor.h"


namespace btte_piece_selection
{

typedef std::set<int> IntSet;
typedef IntSet::iterator IntSetIter;

typedef enum class TypePieceSelect
{
    RANDOM,
    RAREST_FIRST,
    USER_DEFINED
} PieceSelect_T;

class IPieceSelection
{
  public:
    IPieceSelection();
    virtual ~IPieceSelection();
    virtual IntSet StartSelection(const int client_pid) = 0;

  protected:
    void CheckNeighbors();
    void CollectNoDownloadPieces();
    bool HavePiece(const int pid, const int piece_no) const;

    IntSet no_download_pieces_set_;
    int selector_pid_;
};

}

#endif // for #ifndef _PIECE_SELECTION_H
