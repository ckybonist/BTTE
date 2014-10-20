#include "error.h"
#include "random.h"
#include "piece.h"


bool g_all_pieces_get = false;

namespace  // enclosing in compile unit
{

bool* AllocPieces(const int NUM_PIECE)
{
    bool* pieces = new bool[NUM_PIECE];
    if (pieces == nullptr)
    {
        ExitError("Memory allocation of pieces is failed!");
    }
    return pieces;
}

}

bool* MakePieces(const int NUM_PIECE)
{
    bool* pieces = AllocPieces(NUM_PIECE);
    for (int c = 0; c < NUM_PIECE; c++)
    {
        pieces[c] = false;
    }
    return pieces;
}
