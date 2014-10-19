#ifndef _ARGS_H
#define _ARGS_H

#include <string>

struct Args
{
    size_t NUM_PEER;
    size_t NUM_SEED;
    size_t NUM_LEECH;
    size_t NUM_PEERLIST;
    size_t NUM_CHOKING;
    size_t NUM_OU;
    size_t NUM_PIECE;
    int TYPE_PEERSELECT;
    int TYPE_PIECESELECT;
    float ARRIVAL_RATE;

    Args() {};
    Args(const std::string filename);
};

#endif // for #ifndef _ARGS_H
