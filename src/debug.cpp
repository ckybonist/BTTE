#include <iostream>

#include "peer.h"
#include "error.h"
#include "debug.h"


using std::cout;
using std::endl;


namespace
{

void PeerInfo(const size_t pid);

void PieceInfo(const size_t pid,
               const size_t NUM_PIECE,
               const size_t NUM_SEED,
               int* counter);

void NeighborInfo(const size_t pid, const size_t NUM_PEERLIST);

}


void ShowDbgInfo(const Args &args)
{
    int* piece_own_counter = new int[args.NUM_PIECE];

    if (nullptr == piece_own_counter)
    {
        ExitError("\nMemory Allocation Fault\n");
    }

    cout.precision(3);

    cout << "\n\n\n\n@ Peer Info: \n\n";

    for (int pid = 0; (size_t)pid < args.NUM_PEER; pid++)
    {
        PeerInfo(pid);

        PieceInfo(pid, args.NUM_PIECE, args.NUM_SEED, piece_own_counter);

        if ((size_t)pid >= args.NUM_SEED)
            NeighborInfo(pid, args.NUM_PEERLIST);

        cout << "\n===========================\n\n";
    }

    //cout << "\nNumber of peers own each piece:\n";
    //for (int i = 0; (size_t)i < args.NUM_PIECE; ++i)
    //{
    //    cout << "Piece #" << i << " : "
    //         << piece_own_counter[i] << endl;
    //}

    cout << endl;

    delete [] piece_own_counter;
}

namespace
{

void PeerInfo(const size_t pid)
{
    ////////////////////////
    // id info
    cout << "Peer ID: " << g_peers[pid].pid << endl;
    cout << "Cluster ID: " << g_peers[pid].cid << endl;

    cout << "\nJoin time (not yet): " << g_peers[pid].join_time << endl;

    if (g_peers[pid].is_seed)
    {
        cout << "Peer type: Seed" << endl;
    }
    else if (g_peers[pid].is_leech)
    {
        cout << "Peer type: Leech" << endl;
    }
    else
    {
        cout << "Peer type: Average" << endl;
    }

    cout << "\nTime per packet: " << g_peers[pid].time_packet << endl;
}

void PieceInfo(const size_t pid,
                      const size_t NUM_PIECE,
                      const size_t NUM_SEED,
                      int* counter)
{
    ///////////////////
    // pieces info
    int piece_count = 0;

    for (int c = 0; (size_t)c < NUM_PIECE; c++)
    {
        if (g_peers[pid].pieces[c])
        {
            ++piece_count;

            if (pid >= NUM_SEED)
                ++counter[c];
        }
    }

    cout << "\nPieces status:\n";
    cout << "    * Get: " << piece_count << endl;
    cout << "    * Empty: " << (NUM_PIECE - piece_count) << endl;
}

void NeighborInfo(const size_t pid, const size_t NUM_PEERLIST)
{
    /////////////////////////
    // neighbors info
    cout << "\nNeighbors info (pid, cid, pg_delay):\n";
    for(int k = 0; (size_t)k < NUM_PEERLIST; k++)
    {
        Neighbor neighbor = g_peers[pid].neighbors[k];
        const int nid = neighbor.id;
        if (nid != -1)
        {
            cout << "    (" << nid
                 << ",  " << g_peers[nid].cid
                 << ",  " << g_peers[nid].neighbor_counts
                 << ",  " << neighbor.pg_delay
                 << ")" << endl;
        }
    }
}

} // unamed namespace
