#include <iostream>

#include "peer.h"
#include "debug.h"

void ShowDbgInfo(const Args &args) {
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

        cout << "\n 4. Time per piece: " << g_peers[i].time_per_piece << endl;

        // piece info
        int piece_count = 0;
        for(int j = 0; j < args.NUM_PIECE; j++) {
            if(true == g_peers[i].pieces[j]) ++piece_count;
        }
        cout << "\n 5. Pieces status:\n";
        cout << "    * Downloaded: " << piece_count << endl;
        cout << "    * Not yet: " << (args.NUM_PIECE - piece_count) << endl;

        cout << "\n 6. Join time (not yet): " << g_peers[i].start_time << endl;

        cout << endl;
        cout << "---------------------------";
        cout << endl;
    }
}
