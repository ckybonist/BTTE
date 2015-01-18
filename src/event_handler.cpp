#include <fstream>
#include <cassert>
#include <cmath>

#include "random.h"
#include "piece.h"
#include "pg_delay.h"
#include "peer_level.h"
#include "algorithm/choking.h"

#include "event_handler.h"


using namespace btte_uniformrand;


namespace
{

typedef std::map<Event::Type4BT, std::string> TBTmapStr;  // debug

void Event2Str(TBTmapStr&);
void CountEvent();
void WriteEventInfo(std::ofstream& ofs,
                    Event const& head,
                    float cur_sys_time);
size_t GetAboriginSize();

TBTmapStr tbt2str;

}


const float slowest_bandwidth = static_cast<float>(g_kPieceSize) / g_kPeerLevel[2].bandwidth.downlink; const float EventHandler::kTimeout_ = 2 * slowest_bandwidth;


EventHandler::EventHandler(PeerManager* const pm, float lambda, float mu)
{
    pm_ = pm;
    lambda_ = lambda;
    mu_ = mu;
    current_time_ = 0.0;
    waiting_time_ = 0.0;
    next_event_idx_ = 0;

    MapEventFuncs();
    MapEventFlow();
}

EventHandler::~EventHandler()
{
    pm_ = nullptr;
}

void EventHandler::MapEventFuncs()
{
    event_func_map_[Event::PEER_JOIN] = &EventHandler::PeerJoinEvent;
    event_func_map_[Event::PEERLIST_REQ_RECV] = &EventHandler::PeerListReqRecvEvent;
    event_func_map_[Event::PIECE_REQ] = &EventHandler::PieceReqEvent;
    event_func_map_[Event::PIECE_REQ_RECV] = &EventHandler::PieceReqRecvEvent;
    event_func_map_[Event::PIECE_ADMIT] = &EventHandler::PieceAdmitEvent;
    event_func_map_[Event::PIECE_GET] = &EventHandler::PieceGetEvent;
    event_func_map_[Event::COMPLETED] = &EventHandler::CompletedEvent;
    event_func_map_[Event::PEER_LEAVE] = &EventHandler::PeerLeaveEvent;
}

void EventHandler::MapEventFlow()
{
    /* Map Functions */
    event_flow_map_[Event::PEER_JOIN]         = &EventHandler::EC_1;
    event_flow_map_[Event::PEERLIST_REQ_RECV] = &EventHandler::EC_2;
    event_flow_map_[Event::PIECE_REQ]         = &EventHandler::EC_3;
    event_flow_map_[Event::PIECE_REQ_RECV]    = &EventHandler::EC_4;
    event_flow_map_[Event::PIECE_ADMIT]       = &EventHandler::EC_5;
    event_flow_map_[Event::PIECE_GET]         = &EventHandler::EC_6;
    event_flow_map_[Event::COMPLETED]         = &EventHandler::EC_7;
    event_flow_map_[Event::PEER_LEAVE]        = &EventHandler::EC_8;
}

void EventHandler::GetNewPeerList(Event const& ev)
{
    const int client_pid = ev.pid;
    const float time = ev.time + g_kTrackerPGDelay;

    Event next_ev(Event::Type::ARRIVAL,
                  Event::PEERLIST_REQ_RECV,
                  ++next_event_idx_,
                  client_pid,
                  time);

    PushArrivalEvent(next_ev);
}

void EventHandler::GenrPieceReqRecvEvents(Event const& ev)
{
    if (ev.need_new_neighbors)  // no any possible peers to request
    {
        GetNewPeerList(ev);
    }
    else
    {
        for (PieceMsg const& msg : ev.req_msgs)
        {
            Peer const& client = g_peers.at(ev.pid);
            const float pg_delay = msg.pg_delay;
            const float time = ev.time + pg_delay;

            Event next_ev(Event::Type::ARRIVAL,
                          Event::PIECE_REQ_RECV,
                          ++next_event_idx_,
                          msg.dest_pid,
                          time);

            PushArrivalEvent(next_ev);
        }
    }
}

void EventHandler::GenrPieceAdmitEvent(Event const& ev, const bool is_first_admit)
{
    float time = 0.0;
    if (is_first_admit)
    {
        // 因為跟前一個事件(Piece-Req-Recv) 的 peer 是一樣的，
        // 而且這是初始的 admit，所以沒有其它要求的 piece 傳輸時間
        time = ev.time;
    }
    else
    {
        // 需等待前面的 admit 要求的 piece 上傳完，連線才會空出來，
        // 因此加上一個 piece 傳輸時間
        Peer const& client = g_peers.at(ev.pid);
        time = ev.time + client.get_trans_time();
    }

    // Generate Piece Admit Events
    Event next_ev(Event::Type::ARRIVAL,
                  Event::PIECE_ADMIT,
                  ++next_event_idx_,
                  ev.pid,
                  time);

    next_ev.admitted_reqs = ev.admitted_reqs;
    PushArrivalEvent(next_ev);
}

void EventHandler::CheckNonProcessedReqs(Event const& ev)
{
    Peer const& client = g_peers.at(ev.pid);
    MsgList const& recv_msgs = client.get_recv_msg_buf();
    if (!recv_msgs.empty())
    {
        //std::cout << "Peer #" << ev.pid
        //          << " complete downloading, but still remain requests\n";

        for (PieceMsg const& msg : recv_msgs)
        {
            Peer& peer = g_peers.at(msg.src_pid);
            peer.remove_send_msg(msg);  // important !

            if (peer.is_complete()) break;

            const float time = ev.time + msg.pg_delay;

            Event next_ev(Event::Type::ARRIVAL,
                          Event::PIECE_REQ,
                          ++next_event_idx_,
                          msg.src_pid,
                          time);

            PushArrivalEvent(next_ev);
        }
    }
}

/* Create Derived Events of Peer-Join Event */
void EventHandler::EC_1(Event const& ev)
{
    // Peer-Join Event
    assert(ev.type_bt == Event::PEER_JOIN);

    // 再產生下一個 Peer-Join Event
    PushPeerJoinEvent(ev);

    const float time = ev.time + g_kTrackerPGDelay;

    Event next_ev(Event::Type::ARRIVAL,
                  Event::PEERLIST_REQ_RECV,
                  ++next_event_idx_,
                  ev.pid,
                  time);

    PushArrivalEvent(next_ev);
}

/* Create Derived Events of Peer-List-Req-Recv Event */
void EventHandler::EC_2(Event const& ev)
{
    assert(ev.type_bt == Event::PEERLIST_REQ_RECV);

    const float time = ev.time + g_kTrackerPGDelay;

    Event next_ev(Event::Type::ARRIVAL,
                  Event::PIECE_REQ,
                  ++next_event_idx_,
                  ev.pid,
                  time);

    PushArrivalEvent(next_ev);
}

/* Create Derived Events of Peer-List-Get Event */
void EventHandler::EC_3(Event const& ev)
{
    assert(ev.type_bt == Event::PIECE_REQ);
    GenrPieceReqRecvEvents(ev);
}

/* Create Derived Events of Piece-Req-Recv Event */
void EventHandler::EC_4(Event const& ev)
{
    assert(ev.type_bt == Event::PIECE_REQ_RECV);
    GenrPieceAdmitEvent(ev, true);
}

/* Create Derived Events of Piece-Admit Event */
void EventHandler::EC_5(Event const& ev)
{
    assert(ev.type_bt == Event::PIECE_ADMIT);

    const int client_pid = ev.pid;  // receiver of request

    // Generate Piece-Get events
    for (PieceMsg const& msg : ev.uploaded_reqs)
    {
        Peer const& peer = g_peers.at(msg.src_pid);  // sender of request

        const float pg_delay = msg.pg_delay;
        const float time = ev.time + pg_delay;

        Event next_ev = Event(Event::Type::ARRIVAL,
                              Event::PIECE_GET,
                              ++next_event_idx_,
                              msg.src_pid,
                              time);

        PushArrivalEvent(next_ev);
    }


    Peer const& client = g_peers.at(client_pid);

    // Generate Next Admit Events
    if (ev.admitted_reqs.size() != 0)
    {
        GenrPieceAdmitEvent(ev, false);
    }
}

/* Create Derived Events of Piece-Get Event */
void EventHandler::EC_6(Event const& ev)
{
    assert(ev.type_bt == Event::PIECE_GET);

    Peer& client = g_peers.at(ev.pid);
    if (ev.is_complete)
    {
        /*
         *  可能之前要求的 piece 送來的時候,
         *  client 已經成為 seeder 了。
         *  因此要避免產生多餘的 Completed 事件
         * */
        if (!client.is_seed())
        {
            client.to_seed();
            Event next_ev = Event(Event::Type::ARRIVAL,
                                  Event::COMPLETED,
                                  ++next_event_idx_,
                                  ev.pid,
                                  ev.time);

            PushArrivalEvent(next_ev);
        }
    }
    else
    {
        Event next_ev(Event::Type::ARRIVAL,
                      Event::PIECE_REQ,
                      ++next_event_idx_,
                      ev.pid,
                      ev.time);

        PushArrivalEvent(next_ev);
    }
}

/* Create Derived Events of Complete Event */
void EventHandler::EC_7(Event const& ev)
{
    assert(ev.type_bt == Event::COMPLETED);

    const float time_interval = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    const float time = ev.time + time_interval;

    Event next_ev(Event::Type::ARRIVAL,
                  Event::PEER_LEAVE,
                  ++next_event_idx_,
                  ev.pid,
                  time);

    PushArrivalEvent(next_ev);
}

void EventHandler::EC_8(Event const& ev)
{
    assert(ev.type_bt == Event::PEER_LEAVE);

    // 檢查是否還有要求在 queue 裡面，如果有就去處理
    // (告知要求者重新產生要訊息）, 但是要先確定要求者還在
    // swarm 裡面。
    // 告知完後， client 就可以 leave swarm 了。
    CheckNonProcessedReqs(ev);
}

void EventHandler::PushInitEvent()
{
    const size_t NUM_SEED = g_btte_args.get_num_seed();
    const size_t NUM_LEECH = g_btte_args.get_num_leech();

    /* 先將每個 leechers 的 Piece-Get 事件產生 */
    int pid = NUM_SEED;
    int ev_idx = 1;
    while (pid < GetAboriginSize())
    {
        const float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
        Event ev(Event::Type::ARRIVAL,
                 Event::PIECE_GET,
                 ev_idx,
                 pid,
                 time);
        event_list_.push_back(ev);

        ++ev_idx;
        ++pid;
    }

    /* 再產生第一個 normal peer 的 Join 事件 */
    const float time = ExpRand(lambda_, Rand(RSC::EVENT_TIME));
    Event first_event(Event::Type::ARRIVAL,
                      Event::PEER_JOIN,
                      ev_idx,
                      pid,
                      time);
    event_list_.push_back(first_event);

    next_event_idx_ = ev_idx;  // init next event index
}

void EventHandler::PushArrivalEvent(Event const& ev)
{
    event_list_.push_back(ev);
    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

void EventHandler::PushDepartureEvent(const Event::Type4BT type_bt,
                                      const int next_index,
                                      const int pid)
{
    float next_etime = current_time_ + ExpRand(mu_, Rand(RSC::EVENT_TIME));

    Event depart_event(Event::Type::DEPARTURE,
                       type_bt,
                       next_index,
                       pid,
                       next_etime);
    event_list_.push_back(depart_event);
    event_list_.sort();  // 確保離開事件不會發生於再抵達事件之前
}

void EventHandler::PushDerivedEvent(Event const& ev)
{
    // 根據此次事件，衍生接下來的事件
    event_flow_map_[ev.type_bt](*this, ev);
}

void EventHandler::PushPeerJoinEvent(Event const& ev)
{
    /// 如果是處理 Peer Join 事件,就再產生下一個 Peer Join 事件
    //  (因為節點加入順序是按照陣列索引）, 直到數量滿足 NUM_PEER
    const int next_join_pid = ev.pid + 1;
    const size_t NUM_PEER = g_btte_args.get_num_peer();
    const size_t kAborigin = GetAboriginSize();

    if ((size_t)next_join_pid < NUM_PEER)
    {
        if (next_join_pid < kAborigin ||
                !g_peers_reg_info[next_join_pid])
        {
            float time = ev.time + ExpRand(lambda_,
                                           Rand(RSC::EVENT_TIME));

            Event event(Event::Type::ARRIVAL,
                                Event::PEER_JOIN,
                                ++next_event_idx_,
                                next_join_pid,
                                time);
            PushArrivalEvent(event);
        }
    }
}

bool EventHandler::ReqTimeout(Event const& ev)
{
    assert(current_time_ > ev.time_req_send);

    // TODO: 紀錄送出要求的時間(time_req_send)(產生要求事件當下的系統時間的 current_time_)，
    //       並且將現在時間(cur_time)減掉 time_req_send
    //       如果大於等於 kTimeout 就視為 Timeout
    bool flag = false;
    if (current_time_ - ev.time_req_send >= kTimeout_)
    {
        flag = true;
        std::cout << "This req-event is timeout" << std::endl;
    }

    return flag;
}

void EventHandler::SendPieceReqs(Event& ev)
{
    ev.req_msgs = pm_->GenrAllPieceReqs(ev.pid);

    // 沒有要求訊息產生，需要新的 PeerList
    if (ev.req_msgs.empty())
    {
        ev.need_new_neighbors = true;
    }
    else
    {
        for (const PieceMsg& msg : ev.req_msgs)
        {
            Peer& client = g_peers.at(ev.pid);
            Peer& peer = g_peers.at(msg.dest_pid);
            peer.push_recv_msg(msg);
        }
    }
}

void EventHandler::PeerJoinEvent(Event& ev)
{
    const size_t kAborigin = GetAboriginSize();

    if (ev.pid >= kAborigin)  // 如不是 leecher，就要新增節點資料
    {
        pm_->NewPeer(ev.pid);
        g_peers.at(ev.pid).set_join_time(ev.time);
        pm_->UpdatePeerRegStatus(PeerManager::ISF::JOIN, ev.pid);
    }
}

void EventHandler::PeerListReqRecvEvent(Event& ev)
{
    Peer const& client = g_peers.at(ev.pid);

    if (g_peers_reg_info[ev.pid] &&
            !client.is_complete())
    {
        pm_->AllotNeighbors(ev.pid);
    }
}

void EventHandler::PieceReqEvent(Event& ev)
{
    // 1. 執行初始的 Piece Selection，並向所有鄰居送出 piece 的要求
    Peer const& client = g_peers.at(ev.pid);

    if (!client.is_complete())
    {
        SendPieceReqs(ev);
    }
}

void EventHandler::PieceReqRecvEvent(Event& ev)
{
    // 檢查要求者的 piece 狀態，只要有一個是完全沒拿到 piece
    // 就做 choking (先不採用）
    ev.admitted_reqs = Choking(ev.pid);
}

void EventHandler::PieceAdmitEvent(Event& ev)
{
    // 將 piece 送給要求者並產生 PieceGet 事件
    if (!ev.admitted_reqs.empty())
    {
        ev.uploaded_reqs = ev.admitted_reqs;
        for (auto const& msg : ev.uploaded_reqs)
        {
            Peer& peer = g_peers.at(msg.src_pid);

            // 得到 piece
            peer.download_piece(msg.piece_no);

            // 刪除每一個要求者中有關於"送出去的要求"的紀錄
            peer.remove_send_msg(msg);
        }

        ev.admitted_reqs.clear();
    }

    // 執行 choking 來產生下一次的 Piece Admit
    const int client_pid = ev.pid;
    ev.admitted_reqs = Choking(client_pid);
}

void EventHandler::PieceGetEvent(Event& ev)
{
    // Check client is complete
    Peer& client = g_peers.at(ev.pid);

    if (pm_->CheckAllPiecesGet(ev.pid))
    {
        ev.is_complete = true;
        if (!client.is_seed())
        {
            client.set_complete_time(ev.time);
        }
    }
}

void EventHandler::CompletedEvent(Event& ev)
{
    // After random time, this peer will leave.
    // Now, nothing to do
    std::cout << "\nPeer #" << ev.pid << " completed !!!";
}

void EventHandler::PeerLeaveEvent(Event& ev)
{
    Peer& client = g_peers.at(ev.pid);
    client.set_in_swarm(false);
    client.set_leave_time(ev.time);
    pm_->UpdatePeerRegStatus(PeerManager::ISF::LEAVE, ev.pid);
}

void EventHandler::ProcessArrival(Event& ev)
{
    total_sys_size_ += system_.size();

    /* Timeout 機制 */
    // 如果 request event 已經 timeout, 直接忽略(跳出函式)
    //if (e.type_bt == Event::PIECE_REQ_RECV && e.is_timeout)
    //{
        // 刪除要求者中 "正在要求中的 peers (on_req_peers)"
        // 這個紀錄裡面的 接收者ID (ev.pid)
        // 這樣避免在重新尋找這個 piece 的目標節點時，找不到當前的接收者
        //g_peers.at(ev.client_pid).erase_on_req_peer(ev.pid);
    //}

    system_.push_back(ev);

    // 處理 System 的頭一個 BT 事件
    // (this->*event_func_map_[ev.type_bt])(ev);  // func ptr version
    event_func_map_[ev.type_bt](*this, ev);  // std::function version

    // 產生衍生事件
    PushDerivedEvent(ev);

    // 如果系統中只有一個事件，就產生離開事件
    if (system_.size() == 1)
    {
        current_time_ = ev.time;
        PushDepartureEvent(ev.type_bt, next_event_idx_, ev.pid);
    }

    const int ev_idx = static_cast<int>(ev.type_bt);
    ++g_event_counter[ev_idx];
}

void EventHandler::ProcessDeparture(Event const& ev)
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

//void EventHandler::ProcessEvent(Event& ev, std::ofstream& ofs)
void EventHandler::ProcessEvent(Event& ev)
{
    //WriteEventInfo(ofs, ev, current_time_);  // debug

    if (ev.type == Event::Type::ARRIVAL)
    {
        // 如果 peer 已經離開，就不去處理其所屬的事件
        const bool in_swarm = g_peers_reg_info[ev.pid];
        if (ev.type_bt != Event::PEER_JOIN && !in_swarm) return;
        else ProcessArrival(ev);
    }
    else
    {
        ProcessDeparture(ev);
    }
}

void EventHandler::StartRoutine()
{
    //const int aborigin = args_.NUM_SEED + args_.NUM_LEECH;
    //const int num_avg_peer = args_.NUM_PEER - aborigin;
    //Event2Str(tbt2str);  // debug
    //std::ofstream ofs;
    //ofs.open("event_log.txt");

    PushInitEvent();

    while (!event_list_.empty())
    {
        Event head = event_list_.front();

        event_list_.pop_front();

        //ProcessEvent(head, ofs);
        ProcessEvent(head);
    }

    CountEvent();
    //ofs.close();
}


namespace
{

size_t GetAboriginSize() {
    const size_t NUM_SEED = g_btte_args.get_num_seed();
    const size_t NUM_LEECH = g_btte_args.get_num_leech();
    const size_t kAborigin = NUM_SEED + NUM_LEECH;
    return kAborigin;
}

void Event2Str(TBTmapStr& tbt2str)
{
    tbt2str[Event::PEER_JOIN] = "Peer-Join Event";
    tbt2str[Event::PEERLIST_REQ_RECV] = "Peer-List-Req-Recv Event";
    tbt2str[Event::PIECE_REQ] = "Piece-Req Event";
    tbt2str[Event::PIECE_REQ_RECV] = "Piece-Req-Recv Event";
    tbt2str[Event::PIECE_ADMIT] = "Piece-Admit Event";
    tbt2str[Event::PIECE_GET] = "Piece-Get Event";
    tbt2str[Event::COMPLETED] = "Completed Event";
    tbt2str[Event::PEER_LEAVE] = "Peer-Leave Event";
}

void CountEvent()
{
    Event2Str(tbt2str);
    std::cout << "\n\n";
    for (int i = 0; i < 8; i++)
    {
        std::cout << tbt2str[static_cast<Event::Type4BT>(i)] << " ";
        std::cout << g_event_counter[i] << std::endl;
    }
    std::cout << "\n\n";
}

void WriteEventInfo(std::ofstream& ofs,
                    Event const& head,
                    float sys_cur_time)
{
    ofs.precision(5);

    Event2Str(tbt2str);
    if (head.type == Event::Type::ARRIVAL)
    {
        ofs << "\nEvent #" << head.index << " arrival at " << head.time;
        ofs << "\nCurrent System time: " << sys_cur_time << "\n";
    }
    else
    {
        ofs << "\nEvent #" << head.index << " departure at " << head.time;
        ofs << "\nCurrent System time: " << sys_cur_time << "\n";
    }
    ofs <<  tbt2str[head.type_bt];
    ofs << "\nBelongs to peer #" << head.pid << "\n\n";
}

}  // namespace
