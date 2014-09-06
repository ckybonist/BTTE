#ifndef _PIECE_H
#define _PIECE_H

#include <iostream>
#include <string>

const int PIECE_SIZE = 512 * 1024 * 8;

struct Piece_t
{
    private:
        int no;
        bool is_receive;

    public:
        Piece_t() { int no = 0; is_receive = false; }
        int getNo() { return no; }
        bool getStatus() { return is_receive; }
        void setNo(int val) { is_receive = val; }
        void setStatus(bool status) { is_receive = status; }

};

Piece_t *makePieces(const int size);

#endif // for #ifndef _PIECE_H
