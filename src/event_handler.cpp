#include <iostream>

#include "random.h"
#include "peer.h"
#include "piece.h"
#include "event_handler.h"

using namespace uniformrand;

int EventHandler::num_arrival = 10000;
int EventHandler::peer_join_counts = 0;

EventHandler::EventHandler(Args args, const PeerManager* pm, float l, float m)
{
    args_ = args;
    pm_ = pm;
    lambda_ = l;
    mu_ = m;
    current_time_ = 0.0;
    waiting_time_ = 0.0;

    MapEvent();
    MapEventDeps();
}

void EventHandler::MapEvent()
{
    event_map_[Event::PEER_JOIN] = &EventHandler::PeerJoinEvent;
    event_map_[Event::PEERLIST_REQ_RECV] = &EventHandler::PeerListReqRecvEvent;
    event_map_[Event::PEERLIST_GET] = &EventHandler::PeerListGetEvent;
    event_map_[Event::REQ_PIECE] = &EventHandler::ReqPieceEvent;
    event_map_[Event::PIECE_ADMIT] = &EventHandler::PieceAdmitEvent;
    event_map_[Event::PIECE_GET] = &EventHandler::PieceGetEvent;
    event_map_[Event::COMPLETED] = &EventHandler::CompletedEvent;
    event_map_[Event::PEER_LEAVE] = &EventHandler::PeerLeaveEvent;
}

void EventHandler::MapEventDeps()
{
    event_deps_map_[Event::PEER_JOIN] = Event::PEER_JOIN;
    event_deps_map_[Event::PEERLIST_REQ_RECV] = Event::PEERLIST_GET;
    event_deps_map_[Event::PEERLIST_GET] = Event::REQ_PIECE;
    event_deps_map_[Event::REQ_PIECE] = Event::PIECE_ADMIT;
    event_deps_map_[Event::PIECE_ADMIT] = Event::PIECE_GET;
    event_deps_map_[Event::PIECE_GET] = Event::COMPLETED;
    event_deps_map_[Event::COMPLETED] = Event::PEER_LEAVE;
    event_deps_map_[Event::PEER_LEAVE] = Event::PEER_LEAVE;
}

void EventHandler::PushInitEvent()
{
    float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    const int pid = args_.NUM_SEED + args_.NUM_LEECH;
    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      0,
                      pid,
                      time);
    event_list_.push_back(first_event);
}

void EventHandler::GetNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt)
{
    if (Event::Type::ARRIVAL == t)
    {
        float time = 0.0;
        if (Event::PEER_JOIN == t_bt)
        {
            time = e.time + ExpRand(lambda_, Rand(RSC::EVENT_TIME));
            ++peer_join_counts;
        }
        else
        {
            time = e.time + g_peers[e.pid].time_packet;
        }

        Event new_event(t, t_bt, e.index + 1, e.pid + 1, time);
        event_list_.push_back(new_event);
    }
    else if (Event::Type::DEPARTURE == t)
    {
        float time = 0.0;
        if (Event::PEER_LEAVE == t_bt)
        {
            time = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));
        }
        else
        {
            time = e.time + g_peers[e.pid].time_packet;
        }

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
    (this->*event_map_[e.type_bt])(e);

    if (e.type_bt != Event::PEER_JOIN || e.type_bt != Event::PEER_LEAVE)
        GetNextEvent(e, Event::Type::ARRIVAL, event_deps_map_[e.type_bt]);

    if (system_.size() == 1)
    {
        current_time_ = e.time;

        GetNextEvent(e, Event::Type::DEPARTURE, e.type_bt);
    }

    event_list_.sort();

    GetNextEvent(e, Event::Type::ARRIVAL, Event::PEER_JOIN);
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

void EventHandler::PeerJoinEvent(Event& e)
{
    const int cid = 0;  // not implemnet yet
    pm_->NewPeer(e.pid, cid, current_time_);
}

void EventHandler::PeerListReqRecvEvent(Event& e)
{
    std::cout << "PID for selecting neighbors: " << e.pid << std::endl;
    pm_->AllotNeighbors(e.pid);
}

void EventHandler::PeerListGetEvent(Event& e)
{

}

void EventHandler::ReqPieceEvent(Event& e)
{
    // for debug usage
    for (int c = 0; c < args_.NUM_PIECE; c++)
    {
        g_peers[e.pid].pieces[c] = true;
    }
}

void EventHandler::PieceAdmitEvent(Event& e)
{

}

void EventHandler::PieceGetEvent(Event& e)
{

}

void EventHandler::CompletedEvent(Event& e)
{

}

void EventHandler::PeerLeaveEvent(Event& e)
{

}

void EventHandler::ProcessEvent(Event& e)
{
    if (e.type == Event::Type::ARRIVAL)
    {
        ProcessArrival(e);
    }
    else if (e.type == Event::Type::DEPARTURE)
    {
        ProcessDeparture(e);
    }
}

void EventHandler::StartRoutine()
{
    PushInitEvent();

    // FIXME: condition will change as all departure events of peer_join
    //        have been created
    //while(!event_list_.empty())
    while (peer_join_counts <= args_.NUM_PEER)
    {
        Event head = event_list_.front();

        event_list_.pop_front();

        ProcessEvent(head);
        //std::cout << "Infinite loop\n";
    }
}
