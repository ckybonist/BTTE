#ifndef _EVENT_H
#define _EVENT_H

class Event
{
public:
    enum class Type;
    enum class Type4BT;

    Event(Type t, Type4BT t_bt, int idx, int pid, float ti);
    bool operator< (const Event& e);

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

    Type type;  // arrival or departure
    Type4BT type_bt;

    int index;
    int pid;
    float time;
};


#endif // for #ifndef _EVENT_H
