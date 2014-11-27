#include <iostream>

#include "peer.h"
#include "error.h"
#include "debug.h"


using std::cout;
using std::endl;


namespace
{

void PeerInfo(const int pid);

void PieceInfo(const int pid,
               const size_t NUM_PIECE,
               const size_t NUM_SEED,
               int* counter);

void NeighborInfo(const int pid,
                  const size_t NUM_PEERLIST);

}


void ShowDbgInfo()
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    const size_t NUM_SEED = g_btte_args.get_num_seed();
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    int* piece_own_counter = new int[NUM_PIECE];
    if (nullptr == piece_own_counter)
        ExitError("Memory Allocation Fault");

    cout.precision(3);
    cout << "\n\n\n\n@ Peer Info: \n\n";

    for (size_t pid = 0; pid < NUM_PEER; pid++)
    {
        PeerInfo(pid);

        PieceInfo(pid, NUM_PIECE, NUM_SEED, piece_own_counter);

        if (pid >= NUM_SEED)
            NeighborInfo(pid, NUM_PEERLIST);

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

void PeerInfo(const int pid)
{
    ////////////////////////
    // id info
    cout << "Peer ID: " << g_peers.at(pid).get_pid() << endl;
    cout << "Cluster ID: " << g_peers.at(pid).get_cid() << endl;
    cout << "\nJoin time (not yet): " << g_peers.at(pid).get_join_time() << endl;

    auto peer = g_peers.at(pid);

    if (peer.get_type() == Peer::SEED)
    {
        cout << "Peer type: Seed" << endl;
    }
    else if (peer.get_type() == Peer::LEECH)
    {
        cout << "Peer type: Leech" << endl;
    }
    else
    {
        cout << "Peer type: Average" << endl;
    }

    auto& bandwidth = peer.get_bandwidth();
    cout << "Upload Bandwidth: " << bandwidth.uplink << endl;
    cout << "Download Bandwidth: " << bandwidth.downlink << endl;
}

void PieceInfo(const int pid,
               const size_t NUM_PIECE,
               const size_t NUM_SEED,
               int* counter)
{
    ///////////////////
    // pieces info
    int piece_count = 0;

    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        if (g_peers.at(pid).get_nth_piece(c))
        {
            ++piece_count;
            if ((size_t)pid >= NUM_SEED) ++counter[c];
        }
    }
    cout << "\nPieces status:\n";
    cout << "    * Get: " << piece_count << endl;
    cout << "    * Empty: " << (NUM_PIECE - piece_count) << endl;
}

void NeighborInfo(const int pid, const size_t NUM_PEERLIST)
{
    /////////////////////////
    // neighbors info
    cout << "\nNeighbors info (pid, cid, neighbor_counts, pg_delay):\n";
    //for(int k = 0; (size_t)k < NUM_PEERLIST; k++)
    for (auto& nei : g_peers.at(pid).get_neighbors())
    {
        //Neighbor neighbor = g_peers.at(pid).neighbors[k];
        //const int nid = neighbor.id;
        const int nid = nei.first;
        Neighbor const& nei_info = nei.second;
        cout << "    (" << nid
             << ",  " << g_peers.at(nid).get_cid()
             << ",  " << g_peers.at(nid).get_neighbor_counts()
             << ",  " << nei_info.pg_delay
             << ")" << endl;
    }
}

} // unamed namespace
