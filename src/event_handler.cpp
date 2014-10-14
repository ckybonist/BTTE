#include <iostream>

#include "random.h"
#include "peer.h"
#include "piece.h"
#include "event_handler.h"

using namespace uniformrand;


int EventHandler::peer_join_counts_ = 0;  // compare with number of average peer

EventHandler::EventHandler(Args args, PeerManager* const pm, float lambda, float mu)
{
    args_ = args;
    pm_ = pm;
    lambda_ = lambda;
    mu_ = mu;
    current_time_ = 0.0;
    waiting_time_ = 0.0;

    MapEvent();
    MapEventDeps();
}

EventHandler::~EventHandler()
{
    pm_ = nullptr;
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
    event_deps_map_[Event::PEER_JOIN] = Event::PEERLIST_REQ_RECV;
    event_deps_map_[Event::PEERLIST_REQ_RECV] = Event::PEERLIST_GET;
    event_deps_map_[Event::PEERLIST_GET] = Event::REQ_PIECE;
    event_deps_map_[Event::REQ_PIECE] = Event::PIECE_ADMIT;
    event_deps_map_[Event::PIECE_ADMIT] = Event::PIECE_GET;
    event_deps_map_[Event::PIECE_GET] = Event::COMPLETED;
    event_deps_map_[Event::COMPLETED] = Event::PEER_LEAVE;
}

void EventHandler::PushInitEvent()
{
    float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    const int initial_pid = args_.NUM_SEED + args_.NUM_LEECH;
    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      0,
                      initial_pid,
                      time);

    event_list_.push_back(first_event);
}

void EventHandler::GetNextEvent(Event& e,
                                Event::Type t,
                                Event::Type4BT t_bt,
                                const int index,
                                const int pid)
{
    if (Event::Type::ARRIVAL == t && Event::Type4BT::PEER_LEAVE != e.type_bt)
    {
        float time_packet = g_peers[e.pid].time_packet;
        float time = GetNextArrivalEventTime(t_bt, time_packet, e.time);

        Event new_event(t, t_bt, index, pid, time);
        event_list_.push_back(new_event);
    }
    else if (Event::Type::DEPARTURE == t)
    {
        float time = GetNextDepartureEventTime();

        Event depart_event(t, t_bt, e.index, e.pid, time);
        event_list_.push_back(depart_event);
    }

    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

float EventHandler::GetNextArrivalEventTime(const Event::Type4BT t_bt,
                                            float time_packet,
                                            const float current_arrival_etime)
{
    float time = 0.0;

    if (Event::PEER_JOIN == t_bt || Event::PEER_LEAVE == t_bt)
    {
        float lambda_time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
        time = current_arrival_etime + lambda_time;
    }
    else
    {
        time = current_arrival_etime + time_packet;
    }

    return time;
}

float EventHandler::GetNextDepartureEventTime()
{
    float time = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));
    return time;
}


void EventHandler::ProcessArrival(Event& e)
{
    total_sys_size_ += system_.size();

    system_.push_back(e);

    ///// 處理 System 的頭一個BT 事件
    (this->*event_map_[e.type_bt])(e);

    int next_event_idx = e.index;

    ///// 如果不是 Peer Leave 事件, 就產生下一個相依事件
    if (e.type_bt != Event::PEER_LEAVE)
    {
        Event::Type4BT derived_event_type = event_deps_map_[e.type_bt];
        //std::cout << "\nDerived event type: " << derived_event_type << "\n";
        GetNextEvent(e,
                     Event::Type::ARRIVAL,
                     derived_event_type,
                     ++next_event_idx,
                     e.pid);
    }

    if (system_.size() == 1)
    {
        current_time_ = e.time;
        GetNextEvent(e, Event::Type::DEPARTURE, e.type_bt);
    }

    event_list_.sort();

    /// 0 5  init event
    /// 0 5 6
    /// 1 5 7
    /// 2 5 8
    /// 3 5 9
    /// 4 -> do not need any peer join events
    /// 持續產生 Peer Join 事件, 直到數量滿足 NUM_PEER
    const int aborigin = (args_.NUM_SEED + args_.NUM_LEECH);
    const int num_avg_peer = args_.NUM_PEER - aborigin;

    const int next_join_pid = peer_join_counts_ + 1 + aborigin;

    if (peer_join_counts_ < num_avg_peer - 1 &&
            !g_in_swarm_set[next_join_pid])
    {
        GetNextEvent(e, Event::Type::ARRIVAL,
                     Event::PEER_JOIN,
                     ++next_event_idx,
                     next_join_pid);
        ++peer_join_counts_;
    }

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
    pm_->NewPeer(e.pid, -1, e.time);
    pm_->CheckInSwarm();
}

void EventHandler::PeerListReqRecvEvent(Event& e)
{
    pm_->AllotNeighbors(e.pid);
}

void EventHandler::PeerListGetEvent(Event& e)
{
    //TODO
}

void EventHandler::ReqPieceEvent(Event& e)
{
    // for debug usage
    for (size_t c = 0; c < args_.NUM_PIECE; c++)
    {
        g_peers[e.pid].pieces[c] = true;
    }
}

void EventHandler::PieceAdmitEvent(Event& e)
{
    //TODO
}
void EventHandler::PieceGetEvent(Event& e)
{
    //TODO
}

void EventHandler::CompletedEvent(Event& e)
{
    //TODO
}

void EventHandler::PeerLeaveEvent(Event& e)
{
    g_peers[e.pid].in_swarm = false;
    pm_->CheckInSwarm();
    //iSetIter idx = g_in_swarm_set.find(e.pid);
    //g_in_swarm_set.erase(idx);
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

static void EventDebugInfo(Event& head)
{
    std::map<Event::Type4BT, std::string> tbt2str;
    tbt2str[Event::PEER_JOIN] = "Peer-Join Event";
    tbt2str[Event::PEERLIST_REQ_RECV] = "Peer-List-Req-Recv Event";
    tbt2str[Event::PEERLIST_GET] = "Peer-List-Get Event";
    tbt2str[Event::REQ_PIECE] = "Req-Piece Event";
    tbt2str[Event::PIECE_ADMIT] = "Piece-Admit Event";
    tbt2str[Event::PIECE_GET] = "Piece-Get Event";
    tbt2str[Event::COMPLETED] = "Completed Event";
    tbt2str[Event::PEER_LEAVE] = "Peer-Leave Event";

    std::cout.precision(5);

    if(head.type == Event::Type::ARRIVAL)
    {
        std::cout << "\nEvent #" << head.index << " arrival at "
                  << head.time << "\n";
    }
    else
    {
        std::cout << "\nEvent #" << head.index << " departure at "
                  << head.time << "\n";
    }
    std::cout << "It is a " << tbt2str[head.type_bt];
    std::cout << "\nThis is event belongs to peer #" << head.pid
              << "\n\n\n";
}

void EventHandler::StartRoutine()
{
    PushInitEvent();

    const int aborigin = args_.NUM_SEED + args_.NUM_LEECH;
    const int num_avg_peer = args_.NUM_PEER - aborigin;

    while(!event_list_.empty())
    {
        Event head = event_list_.front();

        event_list_.pop_front();

        ProcessEvent(head);

        EventDebugInfo(head);
    }
}
