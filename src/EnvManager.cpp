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

void EnvManager::init(std::string configfile) const
{
    Config cfg(configfile);
    int num_peer = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000
    int num_seed = cfg.getValueOfKey<int>("NUM_SEED", 100);
    int num_leech = cfg.getValueOfKey<int>("NUM_LEECH", 100);
    int type_peerSelect = cfg.getValueOfKey<int>("TYPE_PEERSELECT", 100);
    int type_pieceSelect = cfg.getValueOfKey<int>("TYPE_PIECESELECT", 100);

    PeerManager pm(num_peer, num_seed, num_leech);


    //createPeers();
}

void EnvManager::destroy() const
{
}
