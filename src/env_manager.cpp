#include <iostream>

#include "config.h"
#include "env_manager.h"


/* Class definition */
EnvManager& EnvManager::GetInstance() {
	static EnvManager instance;
	return instance;
}

void EnvManager::Init(const std::string filename) {
    /*
    Config cfg(fname);

    args = {
        cfg.GetValueOfKey<int>("NUM_PEER", 5000),  // if num_peer not being read, then set num_peer to 5000
        cfg.GetValueOfKey<int>("NUM_SEED", 100),
        cfg.GetValueOfKey<int>("NUM_LEECH", 100),
        cfg.GetValueOfKey<int>("NUM_PIECE", 256),
        cfg.GetValueOfKey<int>("TYPE_PEERSELECT", 100),
        cfg.GetValueOfKey<int>("TYPE_PIECESELECT", 100)
    };

    pm = PeerManager(args);
    pm.CreatePeers();

    DebugInfo(args.NUM_PEER, args.NUM_PIECE);
    */
}

void EnvManager::Destroy() {
    //pm.DestroyPeers();
}
