#include "random.h"
#include "event_handler.h"

using namespace uniformrand;

EventHandler::EventHandler(Args args, int idx, float l, float m)
{
    this->args = args;
    init_idx = args.NUM_SEED + args.NUM_LEECH;
    lambda = l;
    mu = m;
    current_time = 0.0;
    waiting_time = 0.0;
}

void EventHandler::PushInitEvent()
{
    float time = ExpRand(lambda, Rand(RSC::EVENT_TIME));
    Event first_event(Event::Type::ARRIVAL, Event::Type4BT::PEER_JOIN, init_idx, time);
    event_list.push_back(first_event);
}

void EventHandler::PushNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt)
{
    if (Event::Type::ARRIVAL == t)
    {
        float time = e.time + ExpRand(lambda, Rand(RSC::EVENT_TIME));
        Event new_event(t, t_bt, e.index + 1, time);
        event_list.push_back(new_event);
    }
    else if (Event::Type::DEPARTURE == t)
    {
        float time = current_time + ExpRand(mu, Rand(RSC::EVENT_TIME));
        Event depart_event(t, t_bt, e.index, time);
        event_list.push_back(depart_event);
    }
}

void EventHandler::ProcessEvent(const Event& e)
{
    // Event Dependencies
    switch (e.type_bt)
    {
        case Event::Type4BT::PEER_JOIN:
            ProcessEventPeerJoin(e);
            break;

        case Event::Type4BT::PEER_LIST_REQ_RECV:
            ProcessEventPeerListReqRecv(e);
            break;

        case Event::Type4BT::PEER_LIST_GET:
            ProcessEventPeerListGet(e);
            break;

        case Event::Type4BT::REQ_PIECE:
            ProcessEventReqPiece(e);
            break;

        case Event::Type4BT::PIECE_ADMIT:
            ProcessEventPieceAdmit(e);
            break;

        case Event::Type4BT::PIECE_GET:
            ProcessEventPieceGet(e);
            break;

        case Event::Type4BT::COMPLETED:
            ProcessEventCompleted(e);
            break;

        case Event::Type4BT::PEER_LEAVE:
            ProcessEventPeerLeave(e);
            break;

        default:
            break;
    }

}

void EventHandler::StartRoutine()
{
    PushInitEvent();

    while(!event_list.empty())
    {
        Event head = event_list.front();

        event_list.pop_front();

        ProcessEvent(head);
    }
}
