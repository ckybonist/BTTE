#include <iostream>
#include <fstream>
#include "args.h"
#include "error.h"
#include "random.h"
#include "peer_level.h"
#include "peer.h"

#include "debug.h"


using std::cout;
using std::endl;


namespace
{

void PrintTTInfo(std::ofstream& ofs);

void PrintPeerInfo(std::ofstream& ofs, const int pid);

void PrintPieceInfo(std::ofstream& ofs,
               const int pid,
               const size_t NUM_PIECE,
               const size_t NUM_SEED,
               int* counter);

void PrintNeighborInfo(std::ofstream& ofs,
                  const int pid,
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


void PrintRandSeedsInfo()
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

void PrintSimuArgsInfo()
{
    cout << "Simulation Arguments:\n";
    cout << "NUM_PEER : " << g_btte_args.get_num_peer() << endl;
    cout << "NUM_SEED : " << g_btte_args.get_num_seed() << endl;
    cout << "NUM_LEECH : " << g_btte_args.get_num_leech() << endl;
    cout << "NUM_PEERLIST : " << g_btte_args.get_num_peerlist() << endl;
    cout << "NUM_PIECE : " << g_btte_args.get_num_piece() << endl;

    cout << "NUM_CHOKING : " << g_btte_args.get_num_choking() << endl;
    cout << "NUM_OU (Optimistic Unchoking) : " << g_btte_args.get_num_ou() << endl;

    std::string ns_algo = "";
    switch (g_btte_args.get_type_peerselect())
    {
        case 0:
            ns_algo = "Standard";
            break;
        case 1:
            ns_algo = "Load Balancing";
            break;
        case 2:
            ns_algo = "Cluster Based";
            break;
        default:
            break;
    }
    cout << "Peer Selection Type : " << ns_algo;

    switch (g_btte_args.get_type_pieceselect())
    {
        case 0:
            ns_algo = "Random";
            break;
        case 1:
            ns_algo = "Rarest First";
            break;
        case 2:
            ns_algo = "User Defined";
            break;
        default:
            break;
    }
    cout << "\nPiece Selection Type : " << ns_algo;
    cout << endl << endl;
}

void PrintDbgInfo()
{
    const size_t NUM_PIECE = g_btte_args.get_num_piece();
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    const size_t NUM_SEED = g_btte_args.get_num_seed();
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    int* piece_own_counter = new int[NUM_PIECE];
    if (nullptr == piece_own_counter)
        ExitError("Memory Allocation Fault");

    /* Simulation Arguments Info */
    PrintSimuArgsInfo();

    std::ofstream ofs;
    ofs.open("peer_info.txt");

    /* Transmission Time Info */
    PrintTTInfo(ofs);

    /* Show debug info */
    ofs.precision(3);
    ofs << "@ Peer Info:\n\n";
    for (size_t pid = 0; pid < NUM_PEER; pid++)
    {
        PrintPeerInfo(ofs, pid);

        PrintPieceInfo(ofs,
                       pid,
                       NUM_PIECE,
                       NUM_SEED,
                       piece_own_counter);

        if (pid >= NUM_SEED)
            PrintNeighborInfo(ofs, pid, NUM_PEERLIST);

        Peer const& peer = g_peers.at(pid);
        const float time_to_complete = peer.get_complete_time() -
                                                peer.get_join_time();
        ofs << "\nUsed Time for Getting All Pieces : "
            << time_to_complete << std::endl;

        ofs << "\n===========================\n\n";
    }

    //cout << "\nNumber of peers own each piece:\n";
    //for (int i = 0; (size_t)i < args.NUM_PIECE; ++i)
    //{
    //    cout << "Piece #" << i << " : "
    //         << piece_own_counter[i] << endl;
    //}

    ofs.close();
    delete [] piece_own_counter;
}

namespace
{

void PrintTTInfo(std::ofstream& ofs)
{
    ofs << "@ Transmission Time of Piece:\n";
    for (int i = 0; i < g_kNumLevel; i++)
    {
        const float up_bandwidth = g_kPeerLevel[i].bandwidth.downlink;
        ofs << "   Level " << i << " : "
            << g_kPieceSize / up_bandwidth << std::endl;
    }
    ofs << "\n\n\n";
}

void PrintPeerInfo(std::ofstream& ofs, const int pid)
{
    ofs << "Peer ID: " << g_peers.at(pid).get_pid() << endl;
    ofs << "Cluster ID: " << g_peers.at(pid).get_cid() << endl;
    ofs << "\nJoin time (not yet): " << g_peers.at(pid).get_join_time() << endl;
    ofs << "\nFinish time (not yet): " << g_peers.at(pid).get_complete_time() << endl;
    ofs << "\nDeparture time (not yet): " << g_peers.at(pid).get_leave_time() << endl;

    auto peer = g_peers.at(pid);

    if (peer.get_type() == Peer::SEED)
    {
        ofs << "Peer type: Seed" << endl;
    }
    else if (peer.get_type() == Peer::LEECH)
    {
        ofs << "Peer type: Leech" << endl;
    }
    else
    {
        ofs << "Peer type: Average" << endl;
    }

    auto& bandwidth = peer.get_bandwidth();
    ofs << "Upload Bandwidth (bps): " << bandwidth.uplink << endl;
    ofs << "Download Bandwidth (bps): " << bandwidth.downlink << endl;
}

void PrintPieceInfo(std::ofstream& ofs,
               const int pid,
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
    ofs << "\nPieces status:\n";
    ofs << "    * Get: " << piece_count << endl;
    ofs << "    * Empty: " << (NUM_PIECE - piece_count) << endl;
}

void PrintNeighborInfo(std::ofstream& ofs,
                  const int pid,
                  const size_t NUM_PEERLIST)
{
    /////////////////////////
    // neighbors info
    ofs << "\nNeighbors info (pid, cid, neighbor_counts, pg_delay):\n";
    for (auto& nei : g_peers.at(pid).get_neighbors())
    {
        const int nid = nei.first;
        Neighbor const& nei_info = nei.second;
        ofs << "    (" << nid
            << ",  " << g_peers.at(nid).get_cid()
            << ",  " << g_peers.at(nid).get_neighbor_counts()
            << ",  " << nei_info.pg_delay
            << ")" << endl;
    }
}

} // unamed namespace
