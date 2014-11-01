#include <iostream>
#include <cmath>

#include "random.h"
#include "peer.h"
#include "peer_level.h"
#include "piece.h"
#include "event_handler.h"

using namespace uniformrand;


namespace
{

typedef std::map<Event::Type4BT, std::string> TBTmapStr;  // debug

void Event2Str(TBTmapStr&);
void EventInfo(const Event& head);

}

const float slowest_bandwidth = static_cast<float>(g_kPieceSize) / g_kPeerLevel[2].bandwidth;
const float EventHandler::kTimeout_ = 2 * slowest_bandwidth;
int EventHandler::next_event_idx_ = 0;
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
    event_map_[Event::TIMEOUT_REQ_PIECE] = &EventHandler::ReqPieceEvent;

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
    const int initial_index = 1;
    next_event_idx_ = initial_index;  // init next event index
    const int initial_pid = args_.NUM_SEED + args_.NUM_LEECH;
    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      initial_index,
                      initial_pid,
                      time);

    event_list_.push_back(first_event);
}

void EventHandler::GetNextArrivalEvent(const Event::Type4BT next_tbt,
                                       const int next_index,
                                       const int next_pid,
                                       const float next_etime)
{
    Event new_event(Event::Type::ARRIVAL,
                    next_tbt,
                    next_index,
                    next_pid,
                    next_etime);
    event_list_.push_back(new_event);

    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

void EventHandler::GetNextDepartureEvent(const Event::Type4BT tbt,
                                         const int next_index,
                                         const int orig_pid)
{
    float next_etime = GetNextDepartureEventTime();

    Event depart_event(Event::Type::DEPARTURE,
                       tbt,
                       next_index,
                       orig_pid,
                       next_etime);
    event_list_.push_back(depart_event);
    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

float EventHandler::GetNextArrivalEventTime(const Event::Type4BT orig_tbt,
                                            float time_packet,
                                            const float current_etime)
{
    float time = 0.0;

    if (Event::PEER_JOIN == orig_tbt ||
            Event::PEER_LEAVE == orig_tbt)
    {
        float lambda_time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
        time = current_etime + lambda_time;
    }
    else
    {
        time = current_etime + time_packet;
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

    ///// 處理 System 的頭一個 BT 事件
    (this->*event_map_[e.type_bt])(e);

    ///// 如果不是 Peer Leave 事件, 就產生下一個相依事件
    if (e.type_bt != Event::PEER_LEAVE &&
            e.type_bt != Event::TIMEOUT_REQ_PIECE)
    {
        Event::Type4BT derived_tbt = event_deps_map_[e.type_bt];
        float time_packet = g_peers[e.pid].time_packet;
        float time = GetNextArrivalEventTime(e.type_bt, time_packet, e.time);
        GetNextArrivalEvent(derived_tbt,
                            ++next_event_idx_,
                            e.pid,
                            time);
    }
    event_list_.sort();

    /// 如果是處理 Peer Join 事件,就再產生下一個 Peer Join 事件
    //  (因為節點加入順序是按照要配合陣列索引）, 直到數量滿足 NUM_PEER
    if(e.type_bt == Event::PEER_JOIN)
    {
        const size_t next_join_pid = e.pid + 1;

        if (next_join_pid < args_.NUM_PEER &&
                !g_in_swarm_set[next_join_pid])
        {
            float time_packet = g_peers[e.pid].time_packet;
            float time = GetNextArrivalEventTime(e.type_bt, time_packet, e.time);
            GetNextArrivalEvent(Event::PEER_JOIN,
                                ++next_event_idx_,
                                static_cast<int>(next_join_pid),
                                time);
            ++peer_join_counts_;
        }
    }
    event_list_.sort();

    if (system_.size() == 1)
    {
        current_time_ = e.time;
        GetNextDepartureEvent(e.type_bt, next_event_idx_, e.pid);
    }
}

void EventHandler::ProcessDeparture(Event& e)
{
    system_.pop_front();

    if (system_.size() != 0)
    {
        Event sys_head = system_.front();
        current_time_ = e.time;
        waiting_time_ = waiting_time_ + (current_time_ - sys_head.time);

        GetNextDepartureEvent(sys_head.type_bt,
                              ++next_event_idx_,
                              sys_head.pid);
    }

    event_list_.sort();
}

void EventHandler::PeerJoinEvent(Event& e)
{
    pm_->NewPeer(e.pid, e.time);
    pm_->CheckInSwarm(PeerManager::ISF::JOIN, e.pid);
}

void EventHandler::PeerListReqRecvEvent(Event& e)
{
    pm_->AllotNeighbors(e.pid);
}

void EventHandler::PeerListGetEvent(Event& e)
{
    // TODO
}

void EventHandler::ReqPieceEvent(Event& e)
{
    std::cout << "Peer #" << e.pid << " execute Piece Selection" << "\n";

    //const int req_piece = pm_->GetReqPiece(e.pid);
    const auto req_msgs = pm_->GetPieceReqMsgs(e.pid);

    for (auto it = req_msgs.begin(); it != req_msgs.end(); ++it)
    {
        auto msg = *it;
        std::cout << "Send piece-req msg from peer #"
                  << msg.src_pid << " to peer #"
                  << msg.dest_pid << std::endl;
        std::cout << "Wanted piece: " << msg.piece_no << "\n\n";
    }

    // TODO
    // 2. 預先產生 Timeout request 事件，如果到時收到 piece
    //    就把這個 timeout 事件移除
    //float time_packet = g_peers[e.pid].time_packet;
    //float time = GetNextArrivalEventTime(e.type_bt, time_packet, e.time) + kTimeout_;
    //GetNextArrivalEvent(Event::TIMEOUT_REQ_PIECE,
    //                    ++next_event_idx_,
    //                    e.pid,
    //                    time);
    //event_list_.sort();
}

void EventHandler::PieceAdmitEvent(Event& e)
{
    // TODO
    // Choking
}

void EventHandler::PieceGetEvent(Event& e)
{
    // TODO
    //
}

void EventHandler::CompletedEvent(Event& e)
{
    //TODO
}

void EventHandler::PeerLeaveEvent(Event& e)
{
    g_peers[e.pid].in_swarm = false;
    pm_->CheckInSwarm(PeerManager::ISF::LEAVE, e.pid);
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


namespace
{

void Event2Str(TBTmapStr &tbt2str)
{
    tbt2str[Event::PEER_JOIN] = "Peer-Join Event";
    tbt2str[Event::PEERLIST_REQ_RECV] = "Peer-List-Req-Recv Event";
    tbt2str[Event::PEERLIST_GET] = "Peer-List-Get Event";
    tbt2str[Event::REQ_PIECE] = "Req-Piece Event";
    tbt2str[Event::TIMEOUT_REQ_PIECE] = "Req-Piece Event";
    tbt2str[Event::PIECE_ADMIT] = "Piece-Admit Event";
    tbt2str[Event::PIECE_GET] = "Piece-Get Event";
    tbt2str[Event::COMPLETED] = "Completed Event";
    tbt2str[Event::PEER_LEAVE] = "Peer-Leave Event";
}

void EventInfo(const Event& head)
{
    std::cout.precision(5);

    TBTmapStr tbt2str;
    Event2Str(tbt2str);

    std::cout << std::flush;
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

}

void EventHandler::StartRoutine()
{
    //const int aborigin = args_.NUM_SEED + args_.NUM_LEECH;
    //const int num_avg_peer = args_.NUM_PEER - aborigin;
    PushInitEvent();

    while(!event_list_.empty())
    {
        Event head = event_list_.front();

        event_list_.pop_front();

        ProcessEvent(head);

        EventInfo(head);
    }
}
