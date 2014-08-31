#ifndef _PEERMANAGER_CPP
#define _PEERMANAGER_CPP

#include "Dice.h"

#include "Config.h"

#include "PeerManager.h"

void PeerManager::makeSeeds(Peer_t &peers, int NUM_SEED) const
{
    srand(time(0));
	Dice dice;
}

void PeerManager::makeLeechs(Peer_t &peers, int NUM_LEECH) const
{
    srand(time(0));
	Dice dice;
}

Peer_t& PeerManager::selectPeers() const
{
}

void PeerManager::createPeers () const
{
    Config cfg("../btte.conf");  // read config at root of project
    int NUM_PEER = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000

    Peer_t *peers = new (std::nothrow) Peer[NUM_PEER];
    if(peers == nullptr) { exitWithError("Allocate memory of peers is fault!\n"); }

    // init peer id
    for(int i = 0; i < NUM_PEER; i++) { peers.id = T_to_str<int>(i+1); }

    allotTransTime(peer, NUM_PEER);

    int NUM_SEED = cfg.getValueOfKey<int>("NUM_SEED", 100);
    makeSeeds(peer, NUM_SEED);

    int NUM_LEECH = cfg.getValueOfKey<int>("NUM_LEECH", 100);
    makeLeechs(peer, NUM_LEECH);
}




#endif // for #ifndef _PEERMANAGER_CPP

