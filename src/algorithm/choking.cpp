#include <iostream>

#include "../args.h"
#include "../random.h"
#include "../peer.h"
#include "choking.h"


using namespace uniformrand;


typedef std::list<PieceMsg> MsgList;

namespace
{
    void OptimisticUnchoke(const MsgList& recv_msgs,
                           MsgList& unchoke_list,
                           const size_t buf_size,
                           const int client_pid)
    {
        int pos = Rand(RSC::CHOKING) % buf_size - 1;
        MsgList::const_iterator it = recv_msgs.begin();
        for (size_t i = 0; i < pos; it++, i++)
        unchoke_list.push_back(*it);
    }

    void Unchoke(const MsgList& recv_msgs,
                 MsgList& unchoke_list,
                 const size_t buf_size,
                 const int client_pid)
    {
        int counts;
        bool do_ou = false;
        const size_t NUM_UNCHOKE = g_btte_args.get_num_choking();
        const size_t NUM_OU = g_btte_args.get_num_ou();

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
                              buf_size,
                              client_pid);
        }
    }
}

MsgList Choking(const int client_pid)
{
    Peer const& client = g_peers.at(client_pid);
    MsgList const& recv_msgs = client.get_recv_msg_buf();
    MsgList unchoke_list;

    g_peers.at(client_pid).sort_recv_msg();
    Unchoke(recv_msgs, unchoke_list, recv_msgs.size(), client_pid);

    return unchoke_list;
}
