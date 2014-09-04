#ifndef _PEERSELECTION_H
#define _PEERSELECTION_H

#include "Peer.h"

class IPeerSelection
{
public:
    virtual int& choosePeers() = 0;

protected:
    virtual float& allotPGDelay() = 0;
};


class Random : protected IPeerSelection
{
public:
    int& choosePeers();

protected:
    float& allotPGDelay();
};


class LoadBalance : protected IPeerSelection
{
public:
    int& choosePeers();

protected:
    float& allotPGDelay();
};


class ClusterBased : protected IPeerSelection
{
public:
    int& choosePeers();

protected:
    float& allotPGDelay();

private:
    void setClusterID(Peer_t&);
};

#endif // for #ifndef _PEERSELECTION_H
