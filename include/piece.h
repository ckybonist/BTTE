#ifndef _PIECE_H
#define _PIECE_H

#include <iostream>
//#include <string>

const int k_piece_size = 512 * 1024 * 8;

struct Piece {
    public:
        Piece() {
            no_ = 0;
            exist_ = false;
        }
        inline int get_no () const { return no_; }
        inline bool get_exist () const { return exist_; }
        inline void set_no(int val) { exist_  = val; }
        inline void set_exist(bool status) { exist_ = status; }
    private:
        int no_;
        bool exist_;
};

Piece *MakePieces(const int);
void GetPieceByProb(Piece*, const double&, const int);

#endif // for #ifndef _PIECE_H
