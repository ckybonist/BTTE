#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <map>
#include <list>
#include <functional>

#include "args.h"
#include "peer_manager.h"
#include "event.h"


class EventHandler
{
public:
    EventHandler(Args args, PeerManager* const pm, float lambda, float mu);
    ~EventHandler();
    void StartRoutine();
    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };

private:
    void PushInitEvent();
    void PushDerivedEvent(Event& e);
    void PushPeerJoinEvent(Event& e);
    void PushArrivalEvent(const Event& e);

    void ProcessArrival(Event& e);
    void ProcessDeparture(Event& e);
    void ProcessEvent(Event& e);

    void PushDepartureEvent(const Event::Type4BT type_bt,
                            const int next_index,
                            const int pid);

    float ComputeArrivalEventTime(const Event& e, const Event::Type4BT derived_type_bt);
    float ComputeDepartureEventTime();

    void MapEvents();
    void CreateSingleFlowDependencies();


private:
    bool ReqTimeout(Event& e);

    void PeerJoinEvent(Event& e);
    void PeerListReqRecvEvent(Event& e);
    void PeerListGetEvent(Event& e);
    void PieceReqRecvEvent(Event& e);
    void PieceAdmitEvent(Event& e);
    void PieceGetEvent(Event& e);
    void CompletedEvent(Event& e);
    void PeerLeaveEvent(Event& e);


private:
    typedef void (EventHandler::*Fptr)(Event&);
    typedef std::map<Event::Type4BT, Fptr> FuncMap;

    FuncMap event_map_;
    std::map<Event::Type4BT, Event::Type4BT> event_deps_map_;

    std::list<Event> event_list_;
    std::list<Event> system_;

    // external
    Args args_;
    PeerManager* pm_;

    float lambda_;
    float mu_;
    float total_sys_size_;
    float current_time_;
    float waiting_time_;

    int next_event_idx_;

    static const float kTimeout_;
};

#endif // for #ifndef _EVENT_HANDLER_H
