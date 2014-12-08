#include <iostream>

#include "args.h"
#include "error.h"
#include "random.h"
#include "peer.h"

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

typedef std::map<RSC, std::string> RSmapStr;

void RSC2Str(RSmapStr& rs2str)
{
    rs2str[RSC::PEER_LEVEL] = "Peer Level";
    rs2str[RSC::PROB_LEECH] = "Base Prob. of Leech";
    rs2str[RSC::PROB_PIECE] = "Prob of Each Leech's Piece";
    rs2str[RSC::EVENT_TIME] = "Event Time";
    rs2str[RSC::PGDELAY] = "Propagation Delay";
    rs2str[RSC::STD_PEERSELECT] = "Standard Peer Selection";
    rs2str[RSC::LB_PEERSELECT] = "Load Balancing Peer Selection";
    rs2str[RSC::CB_PEERSELECT] = "Cluster-Based Peer Selection";
    rs2str[RSC::RFP_PIECESELECT] = "Random Piece Selection";
    rs2str[RSC::RF_PIECESELECT] = "Rarest First Piece Selection";
    rs2str[RSC::CHOKING] = "Choking";
    rs2str[RSC::FREE_1] = "FREE";
    rs2str[RSC::FREE_2] = "FREE";
    rs2str[RSC::FREE_3] = "FREE";
    rs2str[RSC::FREE_4] = "FREE";
}

} // anonymous namespace


void PrintRandSeeds()
{
    RSmapStr rs2str;
    RSC2Str(rs2str);

    for (int i = 0; i < g_kNumRSeeds; i++)
    {
        if (i == 11) { std::cout << "\nUnused Rand Seeds: \n"; }
        RSC type_rsc = static_cast<RSC>(i);
        std::string rsc_str = rs2str[type_rsc];
        std::cout << std::left << rsc_str << " : \n";
        std::cout << "\t\t\t\t" << g_rand_grp[i] << "\n\n";
    }
    std::cout << "\n\n";
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

        // Total download time
        Peer const& peer = g_peers.at(pid);
        const float time_to_complete = peer.get_complete_time() - peer.get_join_time();
        cout << "\nUsed Time for Getting All Pieces : "
             << time_to_complete << std::endl;

        cout << "\n===========================\n\n";
    }

    //cout << "\nNumber of peers own each piece:\n";
    //for (int i = 0; (size_t)i < args.NUM_PIECE; ++i)
    //{
    //    cout << "Piece #" << i << " : "
    //         << piece_own_counter[i] << endl;
    //}

    delete [] piece_own_counter;
}

namespace
{

void PeerInfo(const int pid)
{
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
    int piece_count = 0;

    for (size_t c = 0; c < NUM_PIECE; c++)
    {
        if (g_peers.at(pid).get_nth_piece_info(c))
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
