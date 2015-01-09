#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <map>
#include <list>
#include <functional>

#include "event.h"
#include "peer_manager.h"

class EventHandler
{
  public:
    EventHandler(PeerManager* const pm, float lambda, float mu);
    ~EventHandler();

    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };

    void StartRoutine();  // main routine

  private:
    void MapEventFuncs();
    void MapEventFlow();

    void PushInitEvent();
    void PushDerivedEvent(Event const& ev);
    void PushPeerJoinEvent(Event const& ev);
    void PushArrivalEvent(Event const& ev);
    void PushDepartureEvent(const Event::Type4BT type_bt,
                            const int next_index,
                            const int pid);


    void ProcessArrival(Event& ev);
    void ProcessDeparture(Event const& ev);
    //void ProcessEvent(Event& ev, std::ofstream& ofs);
    void ProcessEvent(Event& ev);

    bool ReqTimeout(Event const& ev);   // TODO
    void SendPieceReqs(Event& ev);

    /* Event Creators (EC) */
    void GetNewPeerList(Event const& ev);
    void GenrPieceReqRecvEvents(Event const& ev);

    // 參數 is_first_admit 代表如果當前 admit 的要求是"最新"的就為 true,
    // 反之， 如果是之前 wating 的就為 false。(牽涉到 event 到達時間的計算)
    void GenrPieceAdmitEvent(Event const& ev,
                             const bool is_first_admit);

    void CheckNonProcessedReqs(Event const& ev);

    void EC_1(Event const& ev);  // for Peer-Join event
    void EC_2(Event const& ev);  // for Peer-List-Req-Recv event
    void EC_3(Event const& ev);  // for Piece-Req event
    void EC_4(Event const& ev);  // for Piece-Req-Recv event
    void EC_5(Event const& ev);  // for Piece-Admit event
    void EC_6(Event const& ev);  // for Piece-Get event
    void EC_7(Event const& ev);  // for Completed event
    void EC_8(Event const& ev);  // for Peer-Leave event


    /* Main BitTorrent Events */
    void PeerJoinEvent(Event& ev);
    void PeerListReqRecvEvent(Event& ev);
    void PieceReqEvent(Event& ev);
    void PieceReqRecvEvent(Event& ev);
    void PieceAdmitEvent(Event& ev);
    void PieceGetEvent(Event& ev);
    void CompletedEvent(Event& ev);
    void PeerLeaveEvent(Event& ev);

    typedef std::function<void(EventHandler&, Event&)> FuncProto_1;
    typedef std::function<void(EventHandler&, Event const&)> FuncProto_2;
    typedef std::map<Event::Type4BT, FuncProto_1> FuncMap_1;
    typedef std::map<Event::Type4BT, FuncProto_2> FuncMap_2;

    FuncMap_1 event_func_map_;
    FuncMap_2 event_flow_map_;  // dv : derived event
    std::map<Event::Type4BT, Event::Type4BT> event_map_;

    std::list<Event> event_list_;
    std::list<Event> system_;

    int next_event_idx_;

    // rate
    float lambda_;
    float mu_;

    // statistics
    float total_sys_size_;
    float current_time_;
    float waiting_time_;
    static const float kTimeout_;

    // external
    PeerManager* pm_;
};

#endif // for #ifndef _EVENT_HANDLER_H
