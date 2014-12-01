#include "error.h"
#include "random.h"
#include "piece.h"


bool g_all_pieces_get = false;

PieceMsg::PieceMsg(const int src_pid,
                   const int dest_pid,
                   const int piece_no,
                   const float src_up_bw)
{
    this->src_pid = src_pid;
    this->dest_pid = dest_pid;
    this->piece_no = piece_no;
    this->src_up_bw = src_up_bw;
}

// sort msg by upload bandwidth of src-peer
bool PieceMsg::operator<(PieceMsg const& p) const
{
    return src_up_bw > p.src_up_bw;
}

namespace  // enclosing in compile unit
{

bool* AllocPieces(const int NUM_PIECE)
{
    if (NUM_PIECE < 1)
        ExitError("Given size of piece must greater than 1");

    bool* pieces = new bool[NUM_PIECE];
    if (pieces == nullptr)
        ExitError("Memory allocation of pieces is failed");

    return pieces;
}

}

bool* MakePieces(const int NUM_PIECE)
{
    bool* pieces = AllocPieces(NUM_PIECE);
    for (int c = 0; c < NUM_PIECE; c++)
        pieces[c] = false;

    return pieces;
}
