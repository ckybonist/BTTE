#include <iostream>
#include "error.h"
#include "config.h"
#include "peer_manager.h"
#include "env_manager.h"

static void DebugInfo(const int NUM_PEER, const int NUM_PIECE) {
    using std::cout;
    using std::endl;

    int piece_count;

    for(int i = 0; i < NUM_PEER; i++) {
        // id info
        cout << "Peer ID: " << g_peers[i].id << endl;
        cout << "Cluster where peer belongs to: " << g_peers[i].cid << endl;

        // seed info
        if(true == g_peers[i].is_seed)
            cout << "I'm a seeder" << endl;
        else
            cout << "I'm still downloading" << endl;

        cout << g_peers[i].time_per_piece << endl;

        // pieces info
        /*
        piece_count = 0;
        for(int j = 0; j < NUM_PIECE; j++)
        {
            //if(true == g_peers[i].pieces[j].getStatus()) { ++piece_count; }
            cout << "piece no: " << g_peers[i].pieces[j].getNo() << endl;
            cout << "piece download status: " << g_peers[i].pieces[j].getStatus() << endl;
        }
        */
        //cout << "Number of downloaded pieces: " << piece_count << endl;
        cout << endl << "---------------------------";
        cout << endl;
    }
}

/* Class definition */
EnvManager& EnvManager::GetInstance() {
	static EnvManager instance;
	return instance;
}

void EnvManager::Init(const std::string fname) {
    Config cfg(fname);
    int num_peer = cfg.GetValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000
    int num_seed = cfg.GetValueOfKey<int>("NUM_SEED", 100);
    int num_leech = cfg.GetValueOfKey<int>("NUM_LEECH", 100);
    int num_piece = cfg.GetValueOfKey<int>("NUM_PIECE", 256);
    int type_peerSelect = cfg.GetValueOfKey<int>("TYPE_PEERSELECT", 100);
    int type_pieceSelect = cfg.GetValueOfKey<int>("TYPE_PIECESELECT", 100);

    pm.InitArgs(num_peer, num_seed, num_leech, num_piece);

    pm.CreatePeers();

    DebugInfo(num_peer, num_piece);
}

void EnvManager::Destroy() {
    pm.DestroyPeers();
}
