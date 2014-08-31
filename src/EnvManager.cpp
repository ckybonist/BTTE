#include <iostream>

//#include <new>

//#include <ctime>

#include "Error.h"
#include "Config.h"
#include "PeerManager.h"
#include "EnvManager.h"

/* Class definition */
EnvManager& EnvManager::getInstance()
{
	static EnvManager instance;
	return instance;
}

void EnvManager::init(std::string configfile, PeerManager pm) const
{
    Config cfg(configfile);
    int num_peer = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000
    int num_seed = cfg.getValueOfKey<int>("NUM_SEED", 100);
    int num_leech = cfg.getValueOfKey<int>("NUM_LEECH", 100);

    pm = PeerManager(num_peer, num_seed, num_leech);


    //createPeers();
}

void EnvManager::destroy() const
{
    int num_peer = sizeof(peers) / sizeof(*peers);
    for(int i = 0; i < num_peer; i++)
    {
        delete peers[i].pdelay;
        peers[i].pdelay = nullptr;

        delete peers[i].pieces;
        peers[i].pieces = nullptr;

        delete peers[i].neighbors;
        peers[i].neighbors = nullptr;
    }

    delete [] peers;
    peers = nullptr;
}
