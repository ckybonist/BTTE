#ifndef _PIECE_H
#define _PIECE_H

//                         512      byte
const int g_k_piece_size = 512 * (1024 * 8);

bool* MakePieces(const int NUM_PIECE);

//void GetPieceByProb(bool* pieces,
//                    const double& prob_leech,
//                    const int NUM_PIECE);

extern bool g_all_pieces_get;

#endif // for #ifndef _PIECE_H
