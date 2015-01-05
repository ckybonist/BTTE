#include <iostream>
#include <fstream>

#include "../args.h"
#include "../random.h"
#include "../peer.h"
#include "choking.h"


using namespace btte_uniformrand;


typedef std::list<PieceMsg> MsgList;

namespace
{
    void OptimisticUnchoke(const MsgList& recv_msgs,
                           MsgList& unchoke_list,
                           const int client_pid)
    {
        const size_t buf_size = recv_msgs.size();
        PieceMsg msg = RandChooseElementInContainer(RSC::CHOKING, recv_msgs);
        unchoke_list.push_back(msg);
    }

    void Unchoke(const MsgList& recv_msgs,
                 MsgList& unchoke_list,
                 const int client_pid)
    {
        int counts;
        bool do_ou = false;
        const size_t NUM_UNCHOKE = g_btte_args.get_num_choking();
        const size_t NUM_OU = g_btte_args.get_num_ou();
        const size_t buf_size = recv_msgs.size();

        if (buf_size >= NUM_UNCHOKE + NUM_OU)
        {
            counts = NUM_UNCHOKE;
            do_ou = true;
        }
        else
        {
            counts = buf_size;
        }

        for (size_t i = 0; i < counts; i++)
        {
            PieceMsg msg = recv_msgs.front();
            unchoke_list.push_back(msg);

            Peer& client = g_peers.at(client_pid);
            client.pop_recv_msg();
        }

        if(do_ou)
        {
            OptimisticUnchoke(recv_msgs,
                              unchoke_list,
                              client_pid);
        }
    }
}

MsgList Choking(const int client_pid)
{
    Peer const& client = g_peers.at(client_pid);
    MsgList const& recv_msgs = client.get_recv_msg_buf();
    MsgList tmp_recv_msgs = recv_msgs;
    MsgList unchoke_list;

    // Sort request msgs by upload bandwidth of sender
    g_peers.at(client_pid).sort_recv_msg();

    // Do choking
    Unchoke(recv_msgs, unchoke_list, client_pid);

    // DEBUG
    std::ofstream ofs;
    ofs.open("choking_log.txt", std::fstream::app);
    ofs << "接收到的要求 :\n";
    ofs << "<src>  <piece>  <upload bandwidth of src>\n";
    for (auto const& msg : tmp_recv_msgs)
    {
        ofs << msg.src_pid << ' '
            << msg.piece_no << ' '
            << msg.src_up_bw << std::endl;
    }

    ofs << "\n\n";

    ofs << "Unchoke(同意分享) 的要求：\n";
    ofs << "<src>  <piece>  <upload bandwidth of src>\n";
    for (auto const& msg : unchoke_list)
    {
        ofs << msg.src_pid << ' '
            << msg.piece_no << ' '
            << msg.src_up_bw << std::endl;
    }

    ofs << "\n--------------------\n";

    return unchoke_list;
}
