#ifndef _EVENT_H
#define _EVENT_H

class Event
{
public:
    enum class Type
    {
        DEPARTURE,
        ARRIVAL
    };

    enum class Type4BT
    {
        PEER_JOIN = 0,
        PEER_LIST_REQ_RECV,
        PEER_LIST_GET,
        REQ_PIECE,
        PIECE_ADMIT,
        PIECE_GET,
        COMPLETED,
        PEER_LEAVE,
    };

    Event(Type t, Type4BT t_bt, int idx, int pid, float ti);

    Type type;  // arrival or departure
    Type4BT type_bt;
    int index;
    float time;

    int pid;

    bool operator< (const Event& e);
};


#endif // for #ifndef _EVENT_H
