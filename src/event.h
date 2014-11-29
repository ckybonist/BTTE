#ifndef _EVENT_H
#define _EVENT_H

#include <deque>

#include "piece.h"
#include "peer.h"

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
    Event(Type t,        // arrival, departure
          Type4BT t_bt,  // type of BT event
          int index,
          int pid,       // which peer belongs to this event
          float ti);     // arrival time of event

    bool operator<(const Event& e);

    Type type;  // arrival, departure
    Type4BT type_bt;

    // Base of Event
    int index;
    int pid;
    float time;

    // Propagation delay with associated neighbor.
    // Use for computing arrival time of event.
    float pg_delay;

    // Piece-related
    std::list<PieceMsg> req_msgs;
    std::list<PieceMsg> admitted_reqs;    // request being admitted
    std::list<PieceMsg> uploaded_reqs;  // request-infos that need to upload piece to
    float time_req_send;

    // TRUE, if no target pieces found when
    // executing Piece Selection
    bool need_new_neighbors;

    bool is_complete;  // all pieces got
};


#endif // for #ifndef _EVENT_H
