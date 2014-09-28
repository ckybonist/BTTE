#ifndef _ARGS_H
#define _ARGS_H

#include <string>

struct Args
{
    int NUM_PEER;
    int NUM_SEED;
    int NUM_LEECH;
    int NUM_PEERLIST;
    int NUM_CHOKING;
    int NUM_OU;
    int NUM_PIECE;
    int TYPE_PEERSELECT;
    int TYPE_PIECESELECT;
    float ARRIVAL_RATE;

    Args() {};
    Args(const std::string filename);
};

#endif // for #ifndef _ARGS_H
