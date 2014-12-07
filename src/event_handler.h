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
    void StartRoutine();
    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };

  private:
    void MapEventFuncs();
    void MapEventFlow();
    void MapFlowDownEventDependencies();

    void PushInitEvent();
    void PushDerivedEvent(Event const& ev);
    void PushPeerJoinEvent(Event const& ev);
    void PushArrivalEvent(Event const& ev);
    void PushDepartureEvent(const Event::Type4BT type_bt,
                            const int next_index,
                            const int pid);


    void ProcessArrival(Event& ev);
    void ProcessDeparture(Event const& ev);
    void ProcessEvent(Event& ev);

    bool ReqTimeout(Event const& ev);   // TODO
    void SendPieceReqs(Event& ev);

    /* Event Creators (EC) */
    void GetNewPeerList(Event const& ev);
    void GenrPieceReqRecvEvents(Event const& ev);

    // is_first_admit: 代表如果當前所 admit 的要求是最新的就為 true, 反之，
    // 如果要求是之前 wating 的就為 false。這牽涉到 event 到達時間的計算。
    void GenrPieceAdmitEvent(Event const& ev, const bool is_first_admit);

    void EC_1(Event const& ev);
    void EC_2(Event const& ev);
    void EC_3(Event const& ev);
    void EC_4(Event const& ev);
    void EC_5(Event const& ev);
    void EC_6(Event const& ev);
    void EC_7(Event const& ev);


    /* Main BitTorrent Events */
    void PeerJoinEvent(Event& ev);
    void PeerListReqRecvEvent(Event& ev);
    void PeerListGetEvent(Event& ev);
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
