#include <iostream>

#include "random.h"
#include "peer.h"
#include "piece.h"
#include "event_handler.h"

using namespace uniformrand;

int EventHandler::num_arrival = 10000;

EventHandler::EventHandler(Args args, const PeerManager* pm, float l, float m) {
    args_ = args;
    pm_ = pm;
    lambda_ = l;
    mu_ = m;
    current_time_ = 0.0;
    waiting_time_ = 0.0;
}

void EventHandler::PushInitEvent()
{
    float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    const int pid = args_.NUM_SEED + args_.NUM_LEECH;
    Event first_event(Event::Type::ARRIVAL,
                      Event::Type4BT::PEER_JOIN,
                      0,
                      pid,
                      time);
    event_list_.push_back(first_event);
}

void EventHandler::GetNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt)
{
    if (Event::Type::ARRIVAL == t)
    {
        float time = e.time + ExpRand(lambda_, Rand(RSC::EVENT_TIME));
        Event new_event(t, t_bt, e.index + 1, e.pid + 1, time);
        event_list_.push_back(new_event);
    }
    else if (Event::Type::DEPARTURE == t)
    {
        float time = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));
        Event depart_event(t, t_bt, e.index, e.pid, time);
        event_list_.push_back(depart_event);

    }
}

void EventHandler::ProcessArrival(Event& e)
{
    total_sys_size_ += system_.size();

    system_.push_back(e);

    std::cout.precision(5);
    std::cout << "\nBT event type: " << static_cast<int>(e.type_bt) << "\n";
    std::cout << "\nPID of BT event: " << e.pid;
    std::cout << "\nEvent time " << e.time << "\n\n";

    // 處理 system 裡第一個 BT 事件的函式
    // TODO : 執行相對應的事件處理函式
    ProcessBTEvent(e);

    Event::Type4BT derived_type = GetDeriveBTEventType(e.type_bt);
    GetNextEvent(e, Event::Type::ARRIVAL, derived_type);

    if (system_.size() == 1)
    {
        current_time_ = e.time;

        GetNextEvent(e, Event::Type::DEPARTURE, e.type_bt);
    }

    event_list_.sort();

    GetNextEvent(e, Event::Type::ARRIVAL, Event::Type4BT::PEER_JOIN);
    event_list_.sort();

}

void EventHandler::ProcessDeparture(Event& e)
{
    system_.pop_front();

    if (system_.size() != 0)
    {
        Event ev = system_.front();
        current_time_ = e.time;
        waiting_time_ = waiting_time_ + (current_time_ - ev.time);

        GetNextEvent(ev, Event::Type::DEPARTURE, e.type_bt);
    }

    event_list_.sort();
}

void EventHandler::ProcessEventPeerJoin(Event& e)
{
    const int cid = 0;  // not implemnet yet
    pm_->NewPeer(e.pid, cid, current_time_);
}

void EventHandler::ProcessEventPeerListReqRecv(Event& e)
{
    std::cout << "PID for selecting neighbors: " << e.pid << std::endl;
    pm_->AllotNeighbors(e.pid);
}

void EventHandler::ProcessEventPeerListGet(Event& e)
{

}

void EventHandler::ProcessEventReqPiece(Event& e)
{
    // for debug usage
    for(int c = 0; c < args_.NUM_PIECE; c++)
    {
        g_peers[e.pid].pieces[c] = true;
    }
}

void EventHandler::ProcessEventPieceAdmit(Event& e)
{

}

void EventHandler::ProcessEventPieceGet(Event& e)
{

}

void EventHandler::ProcessEventCompleted(Event& e)
{

}

void EventHandler::ProcessEventPeerLeave(const Event& e)
{

}

Event::Type4BT EventHandler::GetDeriveBTEventType(Event::Type4BT t_bt)
{
    // Event Dependencies
    switch (t_bt)
    {
        case Event::Type4BT::PEER_JOIN:
            return Event::Type4BT::PEER_LIST_REQ_RECV;
            break;

        case Event::Type4BT::PEER_LIST_REQ_RECV:
            return Event::Type4BT::PEER_LIST_GET;
            break;

        case Event::Type4BT::PEER_LIST_GET: //        ProcessEventPeerListGet(e);
            return Event::Type4BT::REQ_PIECE;
            break;

        case Event::Type4BT::REQ_PIECE:
            return Event::Type4BT::PIECE_ADMIT;
            break;

        case Event::Type4BT::PIECE_ADMIT:
            return Event::Type4BT::PIECE_GET;
            break;

        case Event::Type4BT::PIECE_GET:
            return Event::Type4BT::COMPLETED;
            break;

        case Event::Type4BT::COMPLETED:
            return Event::Type4BT::PEER_LEAVE;
            break;

        case Event::Type4BT::PEER_LEAVE:
            break;

        default:
            break;
    }
}

void EventHandler::ProcessBTEvent(Event &e)
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

        case Event::Type4BT::PEER_LIST_GET: //        ProcessEventPeerListGet(e);
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

void EventHandler::ProcessEvent(Event& e)
{
    if(e.type == Event::Type::ARRIVAL)
    {
        ProcessArrival(e);
    }
    else if(e.type == Event::Type::DEPARTURE)
    {
        ProcessDeparture(e);
    }
}

void EventHandler::StartRoutine()
{
    PushInitEvent();

    // FIXME: condition will change as all departure events of peer_join
    //        have been created
    while(!event_list_.empty())
    {
        Event head = event_list_.front();

        event_list_.pop_front();

        ProcessEvent(head);
        //std::cout << "Infinite loop\n";
    }
}
