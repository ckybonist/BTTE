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
    void MapEventCreators();
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

    float ComputeArrivalEventTime(Event const& ev,
                                  const Event::Type4BT derived_type_bt);
    float ComputeDepartureEventTime();


  private:
    bool ReqTimeout(Event const& ev);   // TODO
    void SendPieceReqs(Event& ev);

    /* Main BitTorrent Events */
    void PeerJoinEvent(Event& ev);
    void PeerListReqRecvEvent(Event& ev);
    void PeerListGetEvent(Event& ev);
    void PieceReqRecvEvent(Event& ev);
    void PieceAdmitEvent(Event& ev);
    void PieceGetEvent(Event& ev);
    void CompletedEvent(Event& ev);
    void PeerLeaveEvent(Event& ev);


  private:
    //typedef void (EventHandler::*Fptr)(Event&);
    typedef std::function<void(EventHandler&, Event&)> FuncProto_1;  // function to process event
    typedef std::function<void(Event const&)> FuncProto_2;  // function to process event
    typedef std::map<Event::Type4BT, FuncProto_1> FuncMap_1;
    typedef std::map<Event::Type4BT, FuncProto_2> FuncMap_2;

    FuncMap_1 event_func_map_;
    FuncMap_2 event_creator_map_;  // dv : derived event
    std::map<Event::Type4BT, Event::Type4BT> event_map_;

    std::list<Event> event_list_;
    std::list<Event> system_;

    // external
    PeerManager* pm_;

    // rate
    float lambda_;
    float mu_;

    // statistics
    float total_sys_size_;
    float current_time_;
    float waiting_time_;

    int next_event_idx_;

    static const float kTimeout_;
};

#endif // for #ifndef _EVENT_HANDLER_H
