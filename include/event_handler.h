#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <list>

#include "args.h"
#include "peer_manager.h"
#include "event.h"



class EventHandler
{
public:
    EventHandler(Args args, const PeerManager* pm, float l, float m);
    ~EventHandler() {};

    void PushInitEvent();
    void GetNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt);
    void ProcessArrival(Event& e);
    void ProcessDeparture(Event& e);
    void ProcessBTEvent(Event& e);
    void ProcessEvent(Event& e);
    void StartRoutine();

    float get_lambda() { return _lambda; };
    float get_mu() { return _mu; };

    static int num_arrival;

protected:
    Event::Type4BT GetDeriveBTEventType(Event::Type4BT t_bt);
    void ProcessEventPeerJoin(Event& e);

    void ProcessEventPeerListReqRecv(Event& e);

    void ProcessEventPeerListGet(Event& e);

    void ProcessEventReqPiece(Event& e);

    void ProcessEventPieceAdmit(Event& e);

    void ProcessEventPieceGet(Event& e);

    void ProcessEventCompleted(Event& e);

    void ProcessEventPeerLeave(const Event& e);

    std::list<Event> _event_list;
    std::list<Event> _system;

    Args _args;

    const PeerManager* _pm;

    float _lambda;
    float _mu;

    float _total_sys_size;
    float _current_time;
    float _waiting_time;
};

#endif // for #ifndef _EVENT_HANDLER_H
