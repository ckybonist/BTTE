#include <iostream>
//#include <cstdlib> // if use normal srand()

#include "error.h"
#include "random.h"

#include "args.h"
#include "peer.h"
#include "peer_manager.h"
//#include "env_manager.h"

void DebugInfo(const Args &args) {
    using std::cout;
    using std::endl;

    cout.precision(3);
    cout << "\n@ Peer Infos: \n\n";
    for(int i = 0; i < args.NUM_PEER; i++) {
        // id info
        cout << " 1. Peer ID: " << g_peers[i].id << endl;
        cout << " 2. Cluster ID: " << g_peers[i].cid << endl;

        if (true == g_peers[i].is_seed)
            cout << " 3. Peer catagory: Seed" << endl;
        else if (true == g_peers[i].is_leech)
            cout << " 3. Peer catagory: Leech" << endl;
        else
            cout << " 3. Peer catagory: Average" << endl;

        cout << "\n 4. Time for downloading one piece: " << g_peers[i].time_per_piece << endl;

        // piece info
        int piece_count = 0;
        for(int j = 0; j < args.NUM_PIECE; j++) {
            if(true == g_peers[i].pieces[j]) ++piece_count;
        }
        cout << "\n 5. Pieces status:\n";
        cout << "    * Downloaded: " << piece_count << endl;
        cout << "    * Not yet: " << (args.NUM_PIECE - piece_count) << endl;

        cout << "\n 6. Join time: " << g_peers[i].start_time << endl;

        cout << endl;
        cout << "---------------------------";
        cout << endl;
    }
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
