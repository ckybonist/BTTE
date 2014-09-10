#include "Error.h"
#include "Random.h"
#include "Piece.h"

static Piece_t *allocPieces(const int NUM_PIECE) {
    Piece_t *pieces = nullptr;
    pieces = new Piece_t[NUM_PIECE];
    if(pieces == nullptr) { exitWithError("Memory allocation of pieces is failed!"); }
    return pieces;
}

Piece_t *makePieces(const int NUM_PIECE) {
    Piece_t *pieces = allocPieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i].setNo(i);
    }
    return pieces;
}

void getPieceByProb(Piece_t *pieces, const double &prob_leech, const int NUM_PIECE) {
    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = (double)uniformdist::rand() / RAND_MAX;
        pieces[i].setStatus((prob_piece < prob_leech));
    }
}
