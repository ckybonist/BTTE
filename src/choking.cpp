#include <iostream>

#include "choking.h"
#include "peer.h"
#include "random.h"
#include "args.h"

using namespace uniformrand;

typedef std::list<PieceMsg> MsgList;

namespace
{
    void OptimisticUnchoke(const MsgList& list, MsgList& unchoke, int buf_size, const int client_pid)
    {
        int ou = Rand(RSC::CHOKING) % buf_size - 1;
        MsgList::const_iterator it = list.begin();
        for (int i = 0; i < ou; it++, i++)
        unchoke.push_back(*it);
    }

    void do_unchoke(const MsgList& list, MsgList& unchoke, const int buf_size, const int client_pid)
    {
        int counts;
        bool do_ou = false;
        const size_t num_unchoke = g_btte_args.get_num_choking();
        const size_t num_ou = g_btte_args.get_num_ou();

        if (buf_size >= num_unchoke + num_ou)
        {
            counts = num_unchoke;
            do_ou = true;
        }
        else
            counts = buf_size;

        for (int i = 0; i < counts; i++)
        {
            PieceMsg msg = list.front();
            unchoke.push_back(msg);
            g_peers.at(client_pid).pop_recv_msg();
        }

        if(do_ou)
            OptimisticUnchoke(list, unchoke, buf_size, client_pid);
    }
}

std::list<PieceMsg> Choking(const int client_pid)
{
    const MsgList& recv_msg = g_peers.at(client_pid).get_recv_msg_buf();
    MsgList unchoke;

    g_peers.at(client_pid).sort_recv_msg();
    do_unchoke(recv_msg, unchoke, recv_msg.size(), client_pid);

    return unchoke;
}
