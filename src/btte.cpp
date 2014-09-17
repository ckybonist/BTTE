#include <iostream>
//#include <cstdlib> // if use normal srand()

#include "args.h"
#include "error.h"
#include "random.h"
#include "peer_manager.h"
//#include "env_manager.h"

void DebugInfo(const Args &args) {
    using std::cout;
    using std::endl;

    cout.precision(6);
    cout << "Info of each peer: \n";
    for(int i = 0; i < args.NUM_PEER; i++) {
        // id info
        cout << "Peer ID: " << g_peers[i].id << endl;
        cout << "Cluster ID: " << g_peers[i].cid << endl;

        // seed info
        if (true == g_peers[i].is_seed)
            cout << "I'm a seeder" << endl;
        else if (true == g_peers[i].is_leech)
            cout << "I'm a leecher" << endl;
        else
            cout << "I'm not ready for downloading" << endl;

        cout << g_peers[i].time_per_piece << endl;

        // piece info
        if(i < (args.NUM_SEED + args.NUM_LEECH)) {
            int piece_count = 0;
            for(int j = 0; j < args.NUM_PIECE; j++) {
                if(true == g_peers[i].pieces[j]) ++piece_count;
            }
            cout << "Number of downloaded pieces: " << piece_count << endl;
            cout << "Number of empty pieces: " << (args.NUM_PIECE - piece_count) << endl;
        }

        cout << endl;
        cout << "---------------------------";
        cout << endl;
    }
    cout << "\n\nJoinable: " << g_joinable;
}

int main(int argc, const char *argv[])
{
    if(argc == 1) {
        ExitError("First argument: path of config file");
    } else if (argc > 2) {
        ExitError("Too much arguments.");
    }

    //uniformdist::srand((unsigned) time(0));  // FIXME: slow speed
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
