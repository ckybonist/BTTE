#ifndef _PIECE_H
#define _PIECE_H

#include <iostream>
//#include <string>
//
//                         512      byte
const int g_k_piece_size = 512 * (1024 * 8);

bool *MakePieces(const int);
void GetPieceByProb(bool*, const double&, const int);

#endif // for #ifndef _PIECE_H
