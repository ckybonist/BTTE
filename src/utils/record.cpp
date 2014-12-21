#include <fstream>
#include <sstream>

#include "../args.h"
#include "../peer.h"
#include "record.h"

namespace
{

std::string GenrFileName()
{
    const int NUM_PEER = g_btte_args.get_num_peer();
    const int NUM_PIECE = g_btte_args.get_num_piece();

    std::string ns_algo;
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

    std::ostringstream oss;
    std::string log_path("../../../log/");
    oss << log_path
        << NUM_PEER << '_'
        << NUM_PIECE << '_'
        << ns_algo << ".txt";
    const std::string filename = oss.str();

    return filename;
}

}

void WriteRecord()
{
    std::ofstream ofs;
    ofs.open(GenrFileName());

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

    ofs.close();
}
