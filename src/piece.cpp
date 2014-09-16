#include "error.h"
#include "random.h"
#include "piece.h"

static bool *AllocPieces(const int NUM_PIECE) {
    bool *pieces = new bool[NUM_PIECE];
    if(pieces == nullptr) {
        ExitError("Memory allocation of pieces is failed!");
    }
    return pieces;
}

bool *MakePieces(const int NUM_PIECE) {
    bool *pieces = AllocPieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i] = false;
    }
    return pieces;
}

void GetPieceByProb(bool *pieces, const double &prob_leech, const int NUM_PIECE) {
    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = (double)uniformdist::rand() / g_k_rand_max;
        pieces[i] = (prob_piece < prob_leech);
    }
}
