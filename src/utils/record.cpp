#include <fstream>
#include <sstream>

#include "../args.h"
#include "../peer.h"
#include "record.h"

namespace
{

std::string GenrFileName(const std::string record_type)
{
    const int NUM_PEER = g_btte_args.get_num_peer();
    const int NUM_PIECE = g_btte_args.get_num_piece();

    std::string ns_algo;
    if (record_type == "peer_sel")
    {
        switch (g_btte_args.get_type_peerselect())
        {
            case 0:
                ns_algo = "Standard";
                break;
            case 1:
                ns_algo = "LoadBalancing";
                break;
            case 2:
                ns_algo = "ClusterBased";
                break;
            case 3:
                ns_algo = "Custom";
            default:
                break;
        }
    }
    else if (record_type == "piece_sel")
    {
        switch (g_btte_args.get_type_pieceselect())
        {
            case 0:
                ns_algo = "Random";
                break;
            case 1:
                ns_algo = "RarestFirst";
                break;
            case 2:
                ns_algo = "Custom";
                break;
            default:
                break;
        }
    }

    std::ostringstream oss;
    std::string log_path("../../../log/raw/");
    oss << log_path
        << NUM_PEER << '_'
        << NUM_PIECE << '_'
        << ns_algo << ".txt";
    const std::string filename = oss.str();

    return filename;
}

}

void WriteRecord(const std::string record_type)
{
    std::ofstream ofs;
    ofs.open(GenrFileName(record_type),
             std::ios_base::app);
    //ofs.open(GenrFileName());

    // write comment at head line
    std::string comment("# peer_id\tjoin_time\tfinish_time\tdeparture_time\n");
    ofs << comment;

    // write record of each peer
    for (Peer const& peer : g_peers)
    {
        const int pid = peer.get_pid();
        const float join_time = peer.get_join_time();
        const float complete_time = peer.get_complete_time();
        const float leave_time = peer.get_leave_time();

        std::ostringstream oss;
        oss << pid << '\t'
            << join_time << '\t'
            << complete_time << '\t'
            << leave_time << '\n';
        std::string record = oss.str();
        ofs << record;
    }
    ofs << ";\n";  // separate record of each simulation

    ofs.close();
}
