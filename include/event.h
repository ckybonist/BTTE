#ifndef _EVENT_H
#define _EVENT_H


class Event
{
public:
    enum class Type
    {
        DEPARTURE,
        ARRIVAL,
        SIZE
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
        SIZE
    };

    Event(Type t, Type4BT t_bt, int idx, int pid, float ti);
    bool operator<(const Event& e);

    Type type;  // arrival or departure
    Type4BT type_bt;

    int index;
    int pid;
    float time;
    float pg_delay;    // propagation delay with associated neighbor

    /* Variables below not use when processing all type of events: */
    // 1. Piece-related
    int piece_no;       // piece no which being admitted (or received)
    int requestor_pid;  // use when generate derived event of PIECE_ADMIT
    bool req_timeout;
    bool am_choking;    // PieceReqRecv event
};


#endif // for #ifndef _EVENT_H
