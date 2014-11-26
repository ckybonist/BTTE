#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <map>
#include <list>
#include <functional>

//#include "args.h"
#include "event.h"
#include "peer_manager.h"


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
    void PushDerivedEvent(const Event& ev);
    void PushPeerJoinEvent(const Event& ev);
    void PushArrivalEvent(const Event& ev);

    void ProcessArrival(Event& ev);
    void ProcessDeparture(const Event& ev);
    void ProcessEvent(Event& ev);

    void PushDepartureEvent(const Event::Type4BT type_bt,
                            const int next_index,
                            const int pid);

    float ComputeArrivalEventTime(const Event& ev, const Event::Type4BT derived_type_bt);
    float ComputeDepartureEventTime();

    void MapEvents();
    void CreateSingleFlowDependencies();


private:
    bool ReqTimeout(const Event& ev);

    void PeerJoinEvent(Event& ev);
    void PeerListReqRecvEvent(Event& ev);
    void PeerListGetEvent(Event& ev);
    void PieceReqRecvEvent(Event& ev);
    void PieceAdmitEvent(Event& ev);
    void PieceGetEvent(Event& ev);
    void CompletedEvent(Event& ev);
    void PeerLeaveEvent(Event& ev);


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
