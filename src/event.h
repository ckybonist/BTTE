#ifndef _EVENT_H
#define _EVENT_H

#include <deque>

#include "piece.h"

class Event
{
public:
    enum class Type
    {
        DEPARTURE,
        ARRIVAL,
    };

    enum Type4BT
    {
        PEER_JOIN,
        PEERLIST_REQ_RECV,
        PEERLIST_GET,
        PIECE_REQ_RECV,
        PIECE_ADMIT,
        PIECE_GET,
        COMPLETED,
        PEER_LEAVE,
    };

    Event() {};
    Event(Type t, Type4BT t_bt, int idx, int pid, float ti);
    bool operator<(const Event& e);

    Type type;  // arrival or departure
    Type4BT type_bt;

    // Base of Event
    int index;
    int pid;
    float time;

    // Propagation delay with associated neighbor.
    // Use for computing arrival time of event.
    float pg_delay;

    // Piece-related
    std::deque<PieceMsg> tmp_req_msgs;
    bool am_choking;    // PieceReqRecv event
    int piece_no;       // piece no which being admitted (or received)
    int client_pid;     // use when generate derived event of PIECE_ADMIT
    float time_req_send;
};


#endif // for #ifndef _EVENT_H
