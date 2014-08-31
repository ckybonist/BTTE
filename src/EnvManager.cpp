#include <iostream>

#include <new>

#include <ctime>

#include "Dice.h"

#include "Error.h"

#include "Peer.h"

#include "EnvManager.h"

/* Class definition */
EnvManager& EnvManager::getInstance()
{
	static EnvManager instance;
	return instance;
}

void EnvManager::init(const PeerManager &pm) const
{
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
