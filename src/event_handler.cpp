#include <iostream>
#include <cassert>
#include <cmath>

#include "peer.h"
#include "piece.h"
//#include "random.h"
#include "random.h"
#include "pg_delay.h"
#include "peer_level.h"
#include "event_handler.h"

using namespace uniformrand;


namespace
{

typedef std::map<Event::Type4BT, std::string> TBTmapStr;  // debug

void Event2Str(TBTmapStr&);
void EventInfo(const Event& head, float cur_sys_time);

}

const float slowest_bandwidth = static_cast<float>(g_kPieceSize) / g_kPeerLevel[2].bandwidth.downlink;
const float EventHandler::kTimeout_ = 2 * slowest_bandwidth;


EventHandler::EventHandler(Args args, PeerManager* const pm, float lambda, float mu)
{
    args_ = args;
    pm_ = pm;
    lambda_ = lambda;
    mu_ = mu;
    current_time_ = 0.0;
    waiting_time_ = 0.0;
    next_event_idx_ = 0;

    MapEvents();
    CreateSingleFlowDependencies();
}

EventHandler::~EventHandler()
{
    pm_ = nullptr;
}

void EventHandler::MapEvents()
{
    event_map_[Event::PEER_JOIN] = &EventHandler::PeerJoinEvent;
    event_map_[Event::PEERLIST_REQ_RECV] = &EventHandler::PeerListReqRecvEvent;
    event_map_[Event::PEERLIST_GET] = &EventHandler::PeerListGetEvent;
    event_map_[Event::PIECE_REQ_RECV] = &EventHandler::PieceReqRecvEvent;
    event_map_[Event::PIECE_ADMIT] = &EventHandler::PieceAdmitEvent;
    event_map_[Event::PIECE_GET] = &EventHandler::PieceGetEvent;
    event_map_[Event::COMPLETED] = &EventHandler::CompletedEvent;
    event_map_[Event::PEER_LEAVE] = &EventHandler::PeerLeaveEvent;
}

void EventHandler::CreateSingleFlowDependencies()
{
    event_deps_map_[Event::PEER_JOIN] = Event::PEERLIST_REQ_RECV;
    event_deps_map_[Event::PEERLIST_REQ_RECV] = Event::PEERLIST_GET;
    event_deps_map_[Event::PEERLIST_GET] = Event::PIECE_REQ_RECV;

    // TODO: The derived event of REQ_PIECE needs to fix
    event_deps_map_[Event::PIECE_REQ_RECV] = Event::PIECE_ADMIT;

    event_deps_map_[Event::PIECE_ADMIT] = Event::PIECE_GET;
    event_deps_map_[Event::PIECE_GET] = Event::COMPLETED;
    event_deps_map_[Event::COMPLETED] = Event::PEER_LEAVE;
}

void EventHandler::PushInitEvent()
{
    const int initial_pid = args_.NUM_SEED + args_.NUM_LEECH;
    const int initial_idx = 1;
    next_event_idx_ = initial_idx;  // init next event index

    float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));

    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      initial_idx,
                      initial_pid,
                      time);

    event_list_.push_back(first_event);
}

void EventHandler::PushArrivalEvent(const Event& ev)
{
    event_list_.push_back(ev);
    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

void EventHandler::PushDepartureEvent(const Event::Type4BT type_bt,
                                      const int next_index,
                                      const int pid)
{
    float next_etime = ComputeDepartureEventTime();

    Event depart_event(Event::Type::DEPARTURE,
                       type_bt,
                       next_index,
                       pid,
                       next_etime);
    event_list_.push_back(depart_event);
    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}


float EventHandler::ComputeArrivalEventTime(const Event& ev, const Event::Type4BT derived_type_bt)
{
    float time = ev.time;
    Event::Type4BT dtbt = derived_type_bt;

    if (dtbt == Event::PEERLIST_REQ_RECV ||
        dtbt == Event::PEERLIST_GET)
    {
        time += g_pg_delay_tracker;
    }
    else if (dtbt == Event::PEER_JOIN ||
             dtbt == Event::PEER_LEAVE)
    {
        time += ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    }
    else if (dtbt == Event::PIECE_ADMIT)
    {
        const float trans_time = g_kPieceSize / g_peers.at(ev.pid).get_bandwidth().downlink;
        time += trans_time;
    }
    else if (dtbt == Event::PIECE_REQ_RECV ||
             dtbt == Event::PIECE_GET)
    {
        time += ev.pg_delay;
    }

    return time;
}

float EventHandler::ComputeDepartureEventTime()
{
    float time = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));
    return time;
}

// TODO: 將以下函式做的事情分散到各個事件中
void EventHandler::PushDerivedEvent(const Event& ev)
{
    int pid = ev.pid;
    Event::Type base_etype = Event::Type::ARRIVAL;
    Event::Type4BT derived_tbt = event_deps_map_[ev.type_bt];  // tbt == type_bt
    float time = ComputeArrivalEventTime(ev, derived_tbt);

    Event next_event = Event(base_etype, derived_tbt, ++next_event_idx_, pid, time);

    //switch (ev.type_bt)
    //{
    //    case Event::PEERLIST_GET:  // generate initial piece-req events
    //        for (const PieceMsg& msg : ev.tmp_req_msgs)
    //        {
    //            next_event.client_pid = msg.src_pid;
    //            next_event.pid = msg.dest_pid;
    //            PushArrivalEvent(next_event);
    //            // prepare another request for timeout situation
    //            //derived_tbt = Event::PIECE_REQ_RECV;
    //            //time += kTimeout_;
    //        }
    //        return;  // already generate derived events
    //        break;

    //    case Event::PIECE_REQ_RECV:
    //        if (ev.am_choking)
    //            return;
    //        else
    //            next_event.client_pid = ev.client_pid;
    //        break;

    //    case Event::PIECE_ADMIT:
    //        next_event.pid = ev.client_pid;
    //        break;

    //    case Event::PIECE_GET:  // if get one piece, generate another piece-req event
    //        if (!g_peers.at(pid).type == SEED)
    //            derived_tbt = Event::PIECE_REQ_RECV;
    //        break;

    //    default:
    //        break;
    //}

    PushArrivalEvent(next_event);
}

void EventHandler::PushPeerJoinEvent(const Event& ev)
{
    /// 如果是處理 Peer Join 事件,就再產生下一個 Peer Join 事件
    //  (因為節點加入順序是按照陣列索引）, 直到數量滿足 NUM_PEER
    const int next_join_pid = ev.pid + 1;

    if ((size_t)next_join_pid < args_.NUM_PEER &&
            !g_in_swarm_set[next_join_pid])
    {
        float time = ComputeArrivalEventTime(ev, Event::PEER_JOIN);
        Event event = Event(Event::Type::ARRIVAL,
                            Event::PEER_JOIN,
                            ++next_event_idx_,
                            next_join_pid,
                            time);
        PushArrivalEvent(event);
    }
}

void EventHandler::ProcessArrival(Event& ev)
{
    total_sys_size_ += system_.size();

    system_.push_back(ev);

    /// 處理 System 的頭一個 BT 事件
    (this->*event_map_[ev.type_bt])(ev);

    /// 如果這個 request event 已經 timeout, 也就代表這個事件已經沒用,
    //  所以不再產生衍生事件，直接跳出函式
    //if (e.type_bt == Event::PIECE_REQ_RECV && e.is_timeout) return;

    // 如果是節點加入事件，就再產生下一個
    if (ev.type_bt == Event::PEER_JOIN) { PushPeerJoinEvent(ev); }

    /// 只要不是 Peer Leave 事件，就產生衍生事件
    if (ev.type_bt != Event::PEER_LEAVE) { PushDerivedEvent(ev); }

    // 如果系統中只有一個事件，就產生離開事件
    if (system_.size() == 1)
    {
        current_time_ = ev.time;
        PushDepartureEvent(ev.type_bt, next_event_idx_, ev.pid);
    }
}

void EventHandler::ProcessDeparture(const Event& ev)
{
    system_.pop_front();

    if (system_.size() != 0)
    {
        Event head = system_.front();
        current_time_ = ev.time;
        waiting_time_ = waiting_time_ + (current_time_ - head.time);
        PushDepartureEvent(head.type_bt, ++next_event_idx_, head.pid);
    }

    event_list_.sort();
}

bool EventHandler::ReqTimeout(const Event& ev)
{
    //assert(current_time_ > e.time_req_send);

    bool flag = false;
    // TODO: 紀錄送出要求的時間(time_req_send)(產生要求事件的系統時的 current-time)，
    //       並且將現在時間(cur_time)減掉 time_req_send
    //       如果大於等於 kTimeout 就視為 Timeout
    if (current_time_ - ev.time_req_send >= kTimeout_)
    {
        std::cout << "This req-event is timeout, so dump it out of the system" << std::endl;
        system_.pop_back();
        flag = true;
    }

    return flag;
}

void EventHandler::PeerJoinEvent(Event& ev)
{
    //pm_->NewPeer(ev.pid, ev.time);
    pm_->NewPeer(ev.pid);
    g_peers.at(ev.pid).set_join_time(current_time_);
    pm_->UpdateSwarmInfo(PeerManager::ISF::JOIN, ev.pid);
}

void EventHandler::PeerListReqRecvEvent(Event& ev)
{
    pm_->AllotNeighbors(ev.pid);
}

void EventHandler::PeerListGetEvent(Event& ev)
{
    // 1. 執行初始的 Piece Selection，並向所有鄰居送出 piece 的要求
    std::cout << "Peer #" << ev.pid << " execute Piece Selection" << "\n";

    ev.tmp_req_msgs = pm_->GetPieceReqMsgs(ev.pid);

    ////for (auto it = ev.req_msgs->begin(); it != ev.req_msgs->end(); ++it)
    //for (const PieceMsg& msg : ev.tmp_req_msgs)
    //{
    //    Peer& client = g_peers.at(ev.pid);      // self peer
    //    Peer& peer = g_peers.at(msg.dest_pid);  // other peer
    //    client.pieces_on_req.insert(msg.piece_no);
    //    peer.recv_msg_buf.push_back(msg);

    //    std::cout << "Sending piece-req msg from peer #" << msg.src_pid << " to peer #"
    //              << msg.dest_pid << std::endl;
    //    std::cout << "Wanted piece: " << msg.piece_no << "\n\n";
    //}

    // TODO
    // 2. 執行第一次 choking，每個 neighbor
    //    會選出 4 or 5 個連線對象並紀錄起來。
    //Choking();
}

void EventHandler::PieceReqRecvEvent(Event& ev)
{
    // 如果已經 timeout 就把這個事件移出系統
    // 並忽略下面的執行程序
    //if (ReqTimeout(e)) return;

    // TODO
    // 接收者收到訊息後，檢查是否 choking 要求者，如果沒有就產生 PieceAdmit 事件
    //for (int i = 0; i < args_.NUM_PEERLIST; ++i)
    //{
    //    const Neighbor& nei = g_peers.at(ev.pid).neighbors[i];
    //    if (nei.first == ev.client_pid)
    //    {
    //        ev.am_choking = nei.conn_states.am_choking;
    //    }
    //}
}

void EventHandler::PieceAdmitEvent(Event& ev)
{
    // TODO : 將 piece 送給要求者並產生 PieceGet 事件
    const Peer& peer = g_peers.at(ev.pid);
    //auto begin = receiver.recv_msg_buf.begin();
    //auto end = receiver.recv_msg_buf.end();
    auto& msg_buf = peer.get_recv_msg_buf();
    for (const PieceMsg& msg : msg_buf)
    {
        if (msg.piece_no == ev.piece_no)
        {
            // send piece to requestor
            Peer& client = g_peers.at(msg.src_pid);
            client.set_nth_piece(msg.piece_no);
            // set pid of requestor into event body
            ev.client_pid = msg.src_pid;
            break;
        }
    }
}

void EventHandler::PieceGetEvent(Event& ev)
{
    // TODO
    // 1. 每取得一個 piece 後，執行 Piece Selection。產生 ReqPiece 事件
    // 2. 取得一個 piece，代表接收者 (收到 client #e.pid 要求的 peer)
    //    空出一個連線，這時就可以執行 Choking 和 Optimistic Unchokinig，
    //    來決定下一個要處理的要求。

    Peer& client = g_peers.at(ev.pid);
    //auto begin = receiver.send_msgs.begin();
    //auto end = receiver.send_msgs.end();
    //for (auto msg = begin; msg != end; ++msg)
    //for (const PieceMsg& msg : client.recv_msg_buf)
    //{
    //    if (msg.piece_no == ev.piece_no)
    //    {
    //        client.pieces_on_req.erase(msg.piece_no);
    //        break;
    //    }
    //}

    //if (pm_->CheckAllPiecesGet(ev.pid))
    //    client.type = SEED;
}

void EventHandler::CompletedEvent(Event& ev)
{
    // After random time, this peer will leave.
    // Now, nothing to do
}

void EventHandler::PeerLeaveEvent(Event& ev)
{
    g_peers.at(ev.pid).set_in_swarm(false);
    g_peers.at(ev.pid).set_leave_time(current_time_);
    pm_->UpdateSwarmInfo(PeerManager::ISF::LEAVE, ev.pid);
}

void EventHandler::ProcessEvent(Event& ev)
{
    if (ev.type == Event::Type::ARRIVAL)
        ProcessArrival(ev);
    else
        ProcessDeparture(ev);
}

namespace
{

void Event2Str(TBTmapStr &tbt2str)
{
    tbt2str[Event::PEER_JOIN] = "Peer-Join Event";
    tbt2str[Event::PEERLIST_REQ_RECV] = "Peer-List-Req-Recv Event";
    tbt2str[Event::PEERLIST_GET] = "Peer-List-Get Event";
    tbt2str[Event::PIECE_REQ_RECV] = "Piece-Req-Recv Event";
    tbt2str[Event::PIECE_ADMIT] = "Piece-Admit Event";
    tbt2str[Event::PIECE_GET] = "Piece-Get Event";
    tbt2str[Event::COMPLETED] = "Completed Event";
    tbt2str[Event::PEER_LEAVE] = "Peer-Leave Event";
}

void EventInfo(const Event& head, float sys_cur_time)
{
    std::cout.precision(5);

    TBTmapStr tbt2str;
    Event2Str(tbt2str);

    std::cout << std::flush;
    if(head.type == Event::Type::ARRIVAL)
    {
        std::cout << "\nEvent #" << head.index << " arrival at " << head.time;
        std::cout << "\nCurrent System time: " << sys_cur_time << "\n";
        if (sys_cur_time > head.time) std::cout << "Timeout";
        else std::cout << "In time";
    }
    else
    {
        std::cout << "\nEvent #" << head.index << " departure at " << head.time;
        std::cout << "\nCurrent System time: " << sys_cur_time << "\n";
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

        EventInfo(head, current_time_);
    }
}
