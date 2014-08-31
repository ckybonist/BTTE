#ifndef _PIECE_H
#define _PIECE_H

#include <iostream>
#include <string>

const int PIECE_SIZE = 512 * 1024 * 8;

struct Piece_t
{
    std::string no;
    bool received;
};

extern Piece& makePieces() const;

#endif // for #ifndef _PIECE_H
