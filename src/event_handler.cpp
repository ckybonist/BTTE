#include <iostream>
#include <cassert>
#include <cmath>

#include "peer.h"
#include "piece.h"
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
    float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));

    const int initial_idx = 1;
    next_event_idx_ = initial_idx;  // init next event index

    const int initial_pid = args_.NUM_SEED + args_.NUM_LEECH;

    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      initial_idx,
                      initial_pid,
                      time);

    event_list_.push_back(first_event);
}

void EventHandler::PushArrivalEvent(const Event::Type4BT type_bt,
                                       const int next_index,
                                       const int pid,
                                       const float next_time)
{
    Event new_event(Event::Type::ARRIVAL,
                    type_bt,
                    next_index,
                    pid,
                    next_time);

    event_list_.push_back(new_event);

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


float EventHandler::ComputeArrivalEventTime(const Event& e, const Event::Type4BT derived_type_bt)
{
    float time = e.time;
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
        const float trans_time = g_kPieceSize / g_peers.at(e.pid).bandwidth.downlink;
        time += trans_time;
    }
    else if (dtbt == Event::PIECE_REQ_RECV ||
             dtbt == Event::PIECE_GET)
    {
        time += e.pg_delay;
    }

    return time;
}

float EventHandler::ComputeDepartureEventTime()
{
    float time = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));
    return time;
}

// TODO: 將以下函式做的事情分散到各個事件中
void EventHandler::PushDerivedEvent(Event& e)
{
    int pid = e.pid;
    Event::Type4BT derived_type_bt = event_deps_map_[e.type_bt];
    float time = ComputeArrivalEventTime(e, derived_type_bt);

    /* Three Special cases below: */
    // 1. Prepare a timeout event in advance
    //if (e.type_bt == Event::PIECE_REQ_RECV &&
    //        !e.am_choking)
    //{
    //    derived_type_bt = Event::PIECE_REQ_RECV;
    //    time += kTimeout_;
    //}

    //// 2. The pid of event will be the piece requestor,
    ////    not the receiver
    //if (e.type_bt == Event::PIECE_ADMIT)
    //{
    //    pid = e.requestor_pid;
    //}

    //// 3. After receiving one piece, checking the peer was seed or not
    //// if it is, then do nothing and generate COMPLETED event
    //// else, generate next REQ_PIECE event
    //if (e.type_bt == Event::PIECE_GET &&
    //        !g_peers.at(e.pid).is_seed)
    //{
    //    derived_type_bt = Event::REQ_PIECE;
    //}

    PushArrivalEvent(derived_type_bt,
                        ++next_event_idx_,
                        pid,
                        time);

    event_list_.sort();
}

void EventHandler::PushPeerJoinEvent(Event& e)
{
    /// 如果是處理 Peer Join 事件,就再產生下一個 Peer Join 事件
    //  (因為節點加入順序是按照陣列索引）, 直到數量滿足 NUM_PEER
    const int next_join_pid = e.pid + 1;

    if ((size_t)next_join_pid < args_.NUM_PEER &&
            !g_in_swarm_set[next_join_pid])
    {
        float time = ComputeArrivalEventTime(e, Event::PEER_JOIN);
        PushArrivalEvent(Event::PEER_JOIN,
                            ++next_event_idx_,
                            next_join_pid,
                            time);
    }

    event_list_.sort();
}

void EventHandler::ProcessArrival(Event& e)
{
    total_sys_size_ += system_.size();

    system_.push_back(e);

    /// 處理 System 的頭一個 BT 事件
    (this->*event_map_[e.type_bt])(e);

    /// 如果這個 request event 已經 timeout, 也就代表這個事件已經沒用,
    //  所以不再產生衍生事件，直接跳出函式
    //if (e.type_bt == Event::REQ_PIECE && e.is_timeout) return;

    // 如果是節點加入事件，就再產生下一個
    if (e.type_bt == Event::PEER_JOIN) { PushPeerJoinEvent(e); }

    /// 只要不是 Peer Leave 事件，就產生此事件的衍生事件
    if (e.type_bt != Event::PEER_LEAVE) { PushDerivedEvent(e); }

    // 如果系統中只有一個事件，就產生離開事件
    if (system_.size() == 1)
    {
        current_time_ = e.time;
        PushDepartureEvent(e.type_bt, next_event_idx_, e.pid);
    }
}

void EventHandler::ProcessDeparture(Event& e)
{
    system_.pop_front();

    if (system_.size() != 0)
    {
        Event head = system_.front();
        current_time_ = e.time;
        waiting_time_ = waiting_time_ + (current_time_ - head.time);

        PushDepartureEvent(head.type_bt, ++next_event_idx_, head.pid);
    }

    event_list_.sort();
}

bool EventHandler::ReqTimeout(Event& e)
{
    //assert(current_time_ > e.time_req_send);

    bool flag = false;
    // TODO: 紀錄送出要求的時間(time_req_send)(產生要求事件的系統時的 current-time)，
    //       並且將現在時間(cur_time)減掉 time_req_send
    //       如果大於等於 kTimeout 就視為 Timeout
    if (current_time_ - e.time_req_send >= kTimeout_)
    {
        std::cout << "This req-event is timeout, so dump it out of the system" << std::endl;
        system_.pop_back();
        flag = true;
    }

    return flag;
}

void EventHandler::PeerJoinEvent(Event& e)
{
    pm_->NewPeer(e.pid, e.time);
    pm_->UpdateSwarmInfo(PeerManager::ISF::JOIN, e.pid);
}

void EventHandler::PeerListReqRecvEvent(Event& e)
{
    pm_->AllotNeighbors(e.pid);
}

void EventHandler::PeerListGetEvent(Event& e)
{
    // 1. 執行初始的 Piece Selection，並向所有鄰居送出 piece 的要求
    std::cout << "Peer #" << e.pid << " execute Piece Selection" << "\n";
    const auto req_msgs = pm_->GetPieceReqMsgs(e.pid);
    for (auto it = req_msgs.begin(); it != req_msgs.end(); ++it)
    {
        auto msg = *it;
        auto sender = g_peers.at(e.pid);
        auto receiver = g_peers.at(msg.dest_pid);

        sender.send_msg_buf.push_back(msg);
        receiver.recv_msg_buf.push_back(msg);

        // generate piece-req-recv event
        //PushArrivalEvent();

        std::cout << "Sending piece-req msg from peer #"
                  << msg.src_pid << " to peer #"
                  << msg.dest_pid << std::endl;
        std::cout << "Wanted piece: " << msg.piece_no << "\n\n";
    }

    event_list_.sort();
    // TODO
    // 2. 執行第一次 choking，每個 neighbor 會選出 4 or 5 個連線對象
    //    並紀錄起來。
    //Choking();
}

void EventHandler::PieceReqRecvEvent(Event& e)
{
    // 如果已經 timeout 就把這個事件移出系統
    // 並忽略下面的執行程序
    //if (ReqTimeout(e)) return;

    // TODO
    // 接收者收到訊息後，檢查是否 choking 要求者，如果沒有就產生 PieceAdmit 事件
    auto receiver = g_peers.at(e.pid);

    for (int i = 0; i < args_.NUM_PEERLIST; i++)
    {
        if (e.requestor_pid == i)
        {
            auto nei = receiver.neighbors[i];
            e.am_choking = nei.conn_states.am_choking;
        }
    }
}

void EventHandler::PieceAdmitEvent(Event& e)
{
    // TODO
    // 將 admit 的 piece 傳給要求者
    // 並產生 PieceGet 事件
    //
    //auto examiner = g_peers.at(e.pid);
    //auto begin = examiner.recv_msgs.begin();
    //auto end = examiner.recv_msgs.end();
    //for (auto msg = begin; msg != end; ++msg)
    //{
    //    if ((*msg).piece_no == e.piece_no)
    //    {
    //        // send piece to requestor
    //        g_peers.at((*msg).src_pid).pieces[(*msg).piece_no] = true;

    //        // set pid of requestor into event body
    //        e.requestor_pid = (*msg).src_pid;

    //        examiner.recv_msgs.erase(msg);
    //        break;
    //    }
    //}

}

void EventHandler::PieceGetEvent(Event& e)
{
    // TODO
    // 1. 每取得一個 piece 後，執行 Piece Selection。產生 ReqPiece 事件
    // 2. 取得一個 piece，代表接收者 (收到 client #e.pid 要求的 peer)
    // 空出一個連線，這時就可以執行 Choking 和 Optimistic Unchokinig，
    // 來決定下一個要處理的要求。


    //auto receiver = g_peers.at(e.pid);
    //auto begin = receiver.send_msgs.begin();
    //auto end = receiver.send_msgs.end();
    //for (auto msg = begin; msg != end; ++msg)
    //{
    //    if ((*msg).piece_no == e.piece_no)
    //    {
    //        receiver.send_msgs.erase(msg);
    //        break;
    //    }
    //}

    //if (pm_->CheckAllPiecesGet(e.pid))
    //{
    //    auto peer = g_peers.at(e.pid);
    //    if (peer.is_leech)
    //        peer.is_leech = false;
    //    g_peers.at(e.pid).is_seed = true;
    //}
}

void EventHandler::CompletedEvent(Event& e)
{
    // After random time, this peer will leave.
    // Now, nothing to do
}

void EventHandler::PeerLeaveEvent(Event& e)
{
    g_peers.at(e.pid).in_swarm = false;
    g_peers.at(e.pid).leave_time = current_time_;
    pm_->UpdateSwarmInfo(PeerManager::ISF::LEAVE, e.pid);
}

void EventHandler::ProcessEvent(Event& e)
{
    if (e.type == Event::Type::ARRIVAL) ProcessArrival(e);
    else ProcessDeparture(e);
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
