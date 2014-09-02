#ifndef _PEERSELECTION_H
#define _PEERSELECTION_H

#include "Dice.h"
#include "Peer.h"

class IPeerSelection
{
public:
    virtual Peer_t& choosePeers() = 0;

protected:
    virtual float& allotPGDelay() = 0;
};

class Random : protected IPeerSelection
{
public:
    Peer_t& choosePeers();

protected:
    float& allotPGDelay();

private:
    Dice dice;
};

class LoadBalance : protected IPeerSelection
{
public:
    Peer_t& choosePeers();

protected:
    float& allotPGDelay();
};

class ClusterBased : protected IPeerSelection
{
public:
    Peer_t& choosePeers();

protected:
    float& allotPGDelay();

private:
    void setClusterID(Peer_t&);
    Dice dice;
};

#endif // for #ifndef _PEERSELECTION_H
