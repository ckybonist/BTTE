#include <iostream>

#include "peer.h"
#include "error.h"
#include "debug.h"


using std::cout;
using std::endl;


static void PntPeerInfo(const int pid);
static void PntPieceInfo(const int pid,
                         const int NUM_PIECE,
                         const int NUM_SEED,
                         int (&counter)[10]);
static void PntNeighborInfo(const int pid, const int NUM_PEERLIST);


void ShowDbgInfo(const Args &args)
{
    //int* piece_own_counter = new int[args.NUM_PIECE];
    int piece_own_counter[10] = {0};
    if(nullptr == piece_own_counter)
    {
        ExitError("\nMemory Allocation Fault\n");
    }

    cout.precision(3);

    cout << "\n@ Peer Infos: \n\n";

    for(int pid = 0; pid < args.NUM_PEER; pid++)
    {
        PntPeerInfo(pid);

        PntPieceInfo(pid, args.NUM_PIECE, args.NUM_SEED, piece_own_counter);

        PntNeighborInfo(pid, args.NUM_PEERLIST);

        ///////////////////////////////////////
        // count how many peers have  each piece
        //
        //if (pid > args.NUM_SEED -1)
        //{
        //    for(int c; c < args.NUM_PIECE; c++)
        //    {
        //        if(true == g_peers[pid].pieces[c])
        //            ++piece_own_counter[c];
        //    }
        //}

        cout << "\n===========================\n";
    }

    cout << "\nNumber of peers own each piece:\n";
    for(int i = 0; i < args.NUM_PIECE; ++i)
    {
        cout << "Piece #" << i << " : "
             << piece_own_counter[i] << endl;
    }

    cout << endl;

    //delete [] piece_own_counter;
}

static void PntPeerInfo(const int pid)
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
}

static void PntPieceInfo(const int pid,
                         const int NUM_PIECE,
                         const int NUM_SEED,
                         int (&counter)[10])
{
    ///////////////////
    // pieces info
    int piece_count = 0;

    for(int c = 0; c < NUM_PIECE; c++)
    {
        if(g_peers[pid].pieces[c])
        {
            ++piece_count;

            if(pid > NUM_SEED - 1)
                ++counter[c];
        }
    }

    cout << "\n 5. Pieces status:\n";
    cout << "    * Downloaded: " << piece_count << endl;
    cout << "    * Not yet: " << (NUM_PIECE - piece_count) << endl;
}

static void PntNeighborInfo(const int pid, const int NUM_PEERLIST)
{
    /////////////////////////
    // neighbors info
    cout << "\n 6. Neighbors info (id, pg_delay):\n";
    for(int k = 0; k < NUM_PEERLIST; k++)
    {
        cout << "    (" << g_peers[pid].neighbors[k].id
             << ", " << g_peers[pid].neighbors[k].pg_delay
             << ")" << endl;
    }
}
