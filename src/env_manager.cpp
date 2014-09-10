#include <iostream>
#include "Error.h"
#include "Config.h"
#include "PeerManager.h"
#include "EnvManager.h"

static void debugInfo(const int NUM_PEER, const int NUM_PIECE) {
    using std::cout;
    using std::endl;

    int piece_count;

    for(int i = 0; i < NUM_PEER; i++) {
        // id info
        cout << "Peer ID: " << peers[i].id << endl;
        cout << "Cluster where peer belongs to: " << peers[i].cid << endl;

        // seed info
        if(true == peers[i].is_seed)
            cout << "I'm a seeder" << endl;
        else
            cout << "I'm still downloading" << endl;

        // pieces info
        /*
        piece_count = 0;
        for(int j = 0; j < NUM_PIECE; j++)
        {
            //if(true == peers[i].pieces[j].getStatus()) { ++piece_count; }
            cout << "piece no: " << peers[i].pieces[j].getNo() << endl;
            cout << "piece download status: " << peers[i].pieces[j].getStatus() << endl;
        }
        */
        //cout << "Number of downloaded pieces: " << piece_count << endl;
        cout << endl << "---------------------------";
        cout << endl;
    }
}

/* Class definition */
EnvManager& EnvManager::getInstance() {
	static EnvManager instance;
	return instance;
}

void EnvManager::init(const std::string configfile) {
    Config cfg(configfile);
    int num_peer = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000
    int num_seed = cfg.getValueOfKey<int>("NUM_SEED", 100);
    int num_leech = cfg.getValueOfKey<int>("NUM_LEECH", 100);
    int num_piece = cfg.getValueOfKey<int>("NUM_PIECE", 256);
    int type_peerSelect = cfg.getValueOfKey<int>("TYPE_PEERSELECT", 100);
    int type_pieceSelect = cfg.getValueOfKey<int>("TYPE_PIECESELECT", 100);

    pm.initArgs(num_peer, num_seed, num_leech, num_piece);

    pm.createPeers();

    debugInfo(num_peer, num_piece);
}

void EnvManager::destroy() {
    pm.destroyPeers(peers);
}
