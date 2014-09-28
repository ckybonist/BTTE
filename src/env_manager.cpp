#include <iostream>

#include "config.h"
#include "env_manager.h"


/* Class definition */
EnvManager& EnvManager::GetInstance()
{
	static EnvManager instance;
	return instance;
}

void EnvManager::Init(const std::string filename)
{
    /*
    Config cfg(fname);

    pm = PeerManager(args);
    pm.CreatePeers();

    DebugInfo(args.NUM_PEER, args.NUM_PIECE);
    */
}

void EnvManager::Destroy()
{
    //pm.DestroyPeers();
}
