#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include <set>

#include "args.h"
#include "neighbor.h"


struct Peer;

namespace peerselection
{

typedef enum class TypePeerSelect
{
    STANDARD = 0,
    LOAD_BALANCING,
    CLUSTER_BASED
} PeerSelect_T;


typedef std::set<int> iSet;
typedef iSet::iterator iSetIter;

class IPeerSelect
{
public:
    IPeerSelect(Args args);
    virtual ~IPeerSelect() {};
    virtual Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) = 0;

protected:
    Args args_;
    int* ids_;
    int* pg_delays_;
};


class Standard : public IPeerSelect
{
public:
    Standard(Args args) : IPeerSelect(args) {};
    ~Standard();

private:
    Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) override;
};


class LoadBalancing : public IPeerSelect
{
public:
    LoadBalancing(Args args) : IPeerSelect(args) {};
    ~LoadBalancing() {};

private:
    Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) override;
    //Neighbor* SelectNeighbors(const int self_pid) override;
};


class ClusterBased : public IPeerSelect
{
public:
    ClusterBased(Args args) : IPeerSelect(args) {};
    ~ClusterBased() {};

private:
    Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) override;
    //Neighbor* SelectNeighbors(const int self_pid) override;
    void setCluster(Peer &peers);
};

}
#endif // for _ABS_PEER_SELECTION_H
