#include "Error.h"
#include "Piece.h"

Piece_t *makePieces(const int NUM_PIECE)
{
    Piece_t *pieces = nullptr;
    pieces = new Piece_t[NUM_PIECE];
    if(pieces == nullptr)
    {
        exitWithError("Memory allocation of pieces is failed");
    }
    for(int i = 0; i < NUM_PIECE; i++) { pieces[i].setNo(i); }
    return pieces;
}
