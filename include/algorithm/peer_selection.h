#ifndef _PEERSELECTION_H
#define _PEERSELECTION_H

#include "peer.h"

class IPeerSelection
{
public:
    virtual int &ChoosePeers() = 0;

protected:
    virtual float &AllotPGDelay() = 0;
};


class Random : protected IPeerSelection
{
public:
    int &ChoosePeers();

protected:
    float &AllotPGDelay();
};


class LoadBalance : protected IPeerSelection
{
public:
    int &ChoosePeers();

protected:
    float &AllotPGDelay();
};


class ClusterBased : protected IPeerSelection
{
public:
    int& ChoosePeers();

protected:
    float& AllotPGDelay();

private:
    void setClusterID(Peer&);
};

#endif // for #ifndef _PEERSELECTION_H
