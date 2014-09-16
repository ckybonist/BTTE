#include "error.h"
#include "random.h"
#include "piece.h"

static Piece *AllocPieces(const int NUM_PIECE) {
    Piece *pieces = nullptr;
    pieces = new Piece[NUM_PIECE];
    if(pieces == nullptr) {
        ExitError("Memory allocation of pieces is failed!");
    }
    return pieces;
}

Piece *MakePieces(const int NUM_PIECE) {
    Piece *pieces = AllocPieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i].set_no(i);
    }
    return pieces;
}

void GetPieceByProb(Piece *pieces, const double &prob_leech, const int NUM_PIECE) {
    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = (double)uniformdist::rand() / g_k_rand_max;
        pieces[i] = (prob_piece < prob_leech);
    }
}
