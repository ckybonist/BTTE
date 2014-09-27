#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

struct Neighbor;

class IPeerSelection
{
public:
    virtual Neighbor* ChoosePeers() = 0;

protected:
    int* NewPGDelay(const int NUM_PEERLIST);
};

#endif // for _ABS_PEER_SELECTION_H
