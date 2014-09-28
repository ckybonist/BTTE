#include <iostream>

#include "peer.h"
#include "debug.h"

void ShowDbgInfo(const Args &args)
{
    using std::cout;
    using std::endl;

    cout.precision(3);
    cout << "\n@ Peer Infos: \n\n";
    for(int pid = 0; pid < args.NUM_PEER; pid++)
    {
        ////////////////////////
        // id info
        cout << " 1. Peer ID: " << g_peers[pid].id << endl;
        cout << " 2. Cluster ID: " << g_peers[pid].cid << endl;

        cout << "\n 6. Join time (not yet): " << g_peers[pid].start_time << endl;

        if (true == g_peers[pid].is_seed)
        {
            cout << " 3. Peer catagory: Seed" << endl;
        }
        else if (true == g_peers[pid].is_leech)
        {
            cout << " 3. Peer catagory: Leech" << endl;
        }
        else
        {
            cout << " 3. Peer catagory: Average" << endl;
        }

        cout << "\n 4. Time per piece: " << g_peers[pid].time_per_piece << endl;

        ///////////////////
        // pieces info
        int piece_count = 0;

        for(int j = 0; j < args.NUM_PIECE; j++)
        {
            if(true == g_peers[pid].pieces[j]) ++piece_count;
        }

        cout << "\n 5. Pieces status:\n";
        cout << "    * Downloaded: " << piece_count << endl;
        cout << "    * Not yet: " << (args.NUM_PIECE - piece_count) << endl;


        /////////////////////////
        // neighbors info
        cout << "\n 6. Neighbors info (id, pg_delay):\n";
        for(int k = 0; k < args.NUM_PEERLIST; k++)
        {
            cout << "    (" << g_peers[pid].neighbors[k].id
                 << ", " << g_peers[pid].neighbors[k].pg_delay
                 << ")" << endl;
        }

        cout << "\n===========================\n";
    }
}
