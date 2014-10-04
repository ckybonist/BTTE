#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <list>

#include "args.h"
#include "event.h"

namespace {

class EventHandler
{
public:
    EventHandler(Args args, int idx, float l, float m);
    ~EventHandler();

    void PushInitEvent();
    void PushNextEvent(Event& e, Event::Type type, Event::Type4BT);
    void ProcessEvent(const Event& e);
    void StartRoutine();

    float get_lambda() { return lambda; };
    float get_mu() { return mu; };

protected:
    //Event::Type GetDeriveEventType(Event::Type e);
    void ProcessEventPeerJoin(const Event& e);

    void ProcessEventPeerListReqRecv(const Event& e);

    void ProcessEventPeerListGet(const Event& e);

    void ProcessEventReqPiece(const Event& e);

    void ProcessEventPieceAdmit(const Event& e);

    void ProcessEventPieceGet(const Event& e);

    void ProcessEventCompleted(const Event& e);

    void ProcessEventPeerLeave(const Event& e);

    std::list<Event> event_list;
    std::list<Event> system;

    Args args;
    int init_idx;  // NUM_SEED + NUM_LEECH

    float lambda;
    float mu;
    float current_time;
    float waiting_time;
};

}

#endif // for #ifndef _EVENT_HANDLER_H
