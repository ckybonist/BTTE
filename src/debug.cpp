#include <iostream>

#include "peer.h"
#include "error.h"
#include "debug.h"


using std::cout;
using std::endl;


static void PeerInfo(const size_t pid);

static void PieceInfo(const size_t pid,
                      const size_t NUM_PIECE,
                      const size_t NUM_SEED,
                      int* counter);

static void NeighborInfo(const size_t pid, const size_t NUM_PEERLIST);


void ShowDbgInfo(const Args &args)
{
    int* piece_own_counter = new int[args.NUM_PIECE];

    if (nullptr == piece_own_counter)
    {
        ExitError("\nMemory Allocation Fault\n");
    }

    cout.precision(3);

    cout << "\n@ Peer Infos: \n\n";

    for (size_t pid = 0; pid < args.NUM_PEER; pid++)
    {
        PeerInfo(pid);

        PieceInfo(pid, args.NUM_PIECE, args.NUM_SEED, piece_own_counter);

        if (pid >= args.NUM_SEED)
            NeighborInfo(pid, args.NUM_PEERLIST);

        cout << "\n===========================\n";
    }

    //cout << "\nNumber of peers own each piece:\n";
    //for (int i = 0; i < args.NUM_PIECE; ++i)
    //{
    //    cout << "Piece #" << i << " : "
    //         << piece_own_counter[i] << endl;
    //}

    cout << endl;

    delete [] piece_own_counter;
}

static void PeerInfo(const size_t pid)
{
    ////////////////////////
    // id info
    cout << " 1. Peer ID: " << g_peers[pid].pid << endl;
    cout << " 2. Cluster ID: " << g_peers[pid].cid << endl;

    cout << "\n 6. Join time (not yet): " << g_peers[pid].join_time << endl;

    if (g_peers[pid].is_seed)
    {
        cout << " 3. Peer catagory: Seed" << endl;
    }
    else if (g_peers[pid].is_leech)
    {
        cout << " 3. Peer catagory: Leech" << endl;
    }
    else
    {
        cout << " 3. Peer catagory: Average" << endl;
    }

    cout << "\n 4. Time per packet: " << g_peers[pid].time_packet << endl;
}

static void PieceInfo(const size_t pid,
                      const size_t NUM_PIECE,
                      const size_t NUM_SEED,
                      int* counter)
{
    ///////////////////
    // pieces info
    int piece_count = 0;

    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        if (g_peers[pid].pieces[c])
        {
            ++piece_count;

            if (pid >= NUM_SEED)
                ++counter[c];
        }
    }

    cout << "\n 5. Pieces status:\n";
    cout << "    * Downloaded: " << piece_count << endl;
    cout << "    * Not yet: " << (NUM_PIECE - piece_count) << endl;
}

static void NeighborInfo(const size_t pid, const size_t NUM_PEERLIST)
{
    /////////////////////////
    // neighbors info
    cout << "\n 6. Neighbors info (id, pg_delay):\n";
    for(size_t k = 0; k < NUM_PEERLIST; k++)
    {
        cout << "    (" << g_peers[pid].neighbors[k].id
             << ", " << g_peers[pid].neighbors[k].pg_delay
             << ")" << endl;
    }
}
