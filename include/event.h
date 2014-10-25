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
        REQ_PIECE,
        TIMEOUT_REQ_PIECE,
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
};


#endif // for #ifndef _EVENT_H
