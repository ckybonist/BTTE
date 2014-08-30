#include <iostream>

#include <new>

#include <ctime>

#include "Dice.h"

#include "Error.h"

#include "ConfigFile.h"

#include "Peer.h"

#include "EnvManager.h"


/*
 * Check amount of each class' peers
 * is reach the target of distributed-rate
 *
 * args:
 * @amount : current amount of n-class' peers
 * @volume : total amount of peers
 *
 * */
static bool isEnough(int cls, int amount, int num_peer)
{
	int limit = (float)DISTRIBUTED[cls] / (float)100 * num_peer;
	return (amount == limit) ? (true) : (false);
}


/* Class definition */
EnvManager& EnvManager::getInstance()
{
	static EnvManager instance;
	return instance;
}

Piece_t& EnvManager::makePieces()
{
}

void EnvManager::allotTransTime(Peer_t &peers)
{
    // Classify peers to three classes
	int exSet[3] = {0};
	int counts[3] = {0};
	int size = sizeof(exSet) / sizeof(*exSet);  // exclusive set size

    srand(time(0));
	Dice dice;

	for(int i = 0; i < num_peer; i++)
    {

		int cls = dice.excludeSet(exSet, size, 1, 3);

		peers.time_piece = TRANS_TIME[cls];

		++counts[cls-1];  // count the amount of class, this must place before the isEnough() check

		 // Check the amount of peers' class is
		 // fullfill our requirement
		if(isEnough(cls-1, counts[cls-1], num_peer))
		{
			if(exSet[cls-1] == 0)
				exSet[cls-1] = cls;
		}
	}
}


void EnvManager::makeSeeds(Peer &peers)
{
}

void EnvManager::makeLeechs(Peer &peers)
{
}

Peer_t& EnvManager::selectPeers()
{
}

void EnvManager::createPeers()
{
    ConfigFile cfg("../btte.conf");  // read config at root of project
    int num_peer = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000

    Peer_t *peers = new (std::nothrow) Peer[num_peer];
    if(peers == nullptr) { exitWithError("Allocate memory of peers is fault!\n"); }

}

void EnvManager::init()
{
/*
    // Print to Test
    using std::cout;
    using std::endl;

    cout << "Total Number of Peers: " << num_peer << endl;
	for(int i = 0; i < 3; i++) {
		cout << "class#" << i << " has "<<counts[i]
		     << " peers" << endl;
    }
    cout << endl;

	cout << "亂數節點 (class#1 ~ class#3):"
	     << endl;

	for(int i = 0; i < num_peer; i++) {
		if (i % 5 == 0) {
			cout << endl;
		}
		cout << peers[i] << " ";
	}

	cout << "\n\n\n";
	cout << "各節點類別的傳輸速度:" << endl << endl;

	for(int i=0; i<3; i++) {
		cout << "C" << (i+1) << " : "
		     << EnvManager::TT[i] << "(s)"
			 << endl;
	}
    */
}

void EnvManager::destroy()
{
}
