#include <iostream>
#include <cstdlib>

#include "args.h"
#include "error.h"
#include "random.h"
#include "peer_manager.h"
//#include "env_manager.h"

void DebugInfo(const Args &args) {
    using std::cout;
    using std::endl;

    int piece_count;

    for(int i = 0; i < args.NUM_PEER; i++) {
        // id info
        cout << "Peer ID: " << g_peers[i].id << endl;
        cout << "Cluster ID: " << g_peers[i].cid << endl;

        // seed info
        if(true == g_peers[i].is_seed)
            cout << "I'm a seeder" << endl;
        else
            cout << "I'm still downloading" << endl;

        cout << g_peers[i].time_per_piece << endl;

        // pieces info
        /*
        piece_count = 0;
        for(int j = 0; j < args.NUM_PIECE; j++)
        {
            //if(true == g_peers[i].pieces[j].getStatus()) { ++piece_count; }
            cout << "piece no: " << g_peers[i].pieces[j].getNo() << endl;
            cout << "piece download status: " << g_peers[i].pieces[j].getStatus() << endl;
        }
        */
        //cout << "Number of downloaded pieces: " << piece_count << endl;
        cout << endl;
        cout << "---------------------------";
        cout << endl;
    }
}

int main(int argc, const char *argv[]) {
    if(argc == 1) {
        ExitError("Type the path of config file for the fisrt argument.");
    } else if (argc > 2) {
        ExitError("Too much arguments.");
    }

    //uniformdist::srand((unsigned) time(0));  // slow speed
    uniformdist::srand(1);  // for testing

    /* read arguments */
    Args args(argv[1]);

    PeerManager pm(args);

    /* start simulating */
    pm.CreatePeers();

    DebugInfo(args);

    pm.DestroyPeers();
    /* stop simulating */

	return 0;
}
