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

    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };

    static int num_arrival;

private:
    Event::Type4BT GetDeriveBTEventType(Event::Type4BT t_bt);
    void ProcessEventPeerJoin(Event& e);

    void ProcessEventPeerListReqRecv(Event& e);

    void ProcessEventPeerListGet(Event& e);

    void ProcessEventReqPiece(Event& e);

    void ProcessEventPieceAdmit(Event& e);

    void ProcessEventPieceGet(Event& e);

    void ProcessEventCompleted(Event& e);

    void ProcessEventPeerLeave(const Event& e);

    std::list<Event> event_list_;
    std::list<Event> system_;

    Args args_;

    const PeerManager* pm_;

    float lambda_;
    float mu_;

    float total_sys_size_;
    float current_time_;
    float waiting_time_;

    static int peer_join_counts;
};

#endif // for #ifndef _EVENT_HANDLER_H
