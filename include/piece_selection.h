#ifndef _PIECE_SELECTION_H
#define _PIECE_SELECTION_H

#include <iostream>
#include <set>

#include "args.h"
#include "random.h"
#include "peer.h"
#include "neighbor.h"


namespace btte_piece_selection
{

typedef std::set<int> iSet;
typedef iSet::iterator iSetIter;


typedef enum class TypePieceSelect
{
    BUILTIN,
    USER_DEFINED_1,
    USER_DEFINED_2,
} PieceSelect_T;


class IPieceSelect
{
public:
    IPieceSelect(Args args);
    virtual ~IPieceSelect();
    virtual int SelectTargetPiece(const int self_pid) = 0;

protected:
    void CheckNeighbors(const int self_pid);
    void SetTargetPieces(const int self_pid);
    iSet targets_set_;

    Args args_;
};

}

#endif // for #ifndef _PIECE_SELECTION_H
