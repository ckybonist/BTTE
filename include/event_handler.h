#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <map>
#include <list>
#include <functional>

#include "args.h"
#include "peer_manager.h"
#include "event.h"


class EventHandler;

typedef void (EventHandler::*Fptr)(Event&);
typedef std::map<Event::Type4BT, Fptr> FuncMap;
typedef std::map<Event::Type4BT, Event::Type4BT> DepMap;  // map for event dependencies

class EventHandler
{
public:
    EventHandler(Args args, const PeerManager* pm, float l, float m);
    ~EventHandler() {};

    void PushInitEvent();
    void GetNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt);
    void ProcessArrival(Event& e);
    void ProcessDeparture(Event& e);
    void ProcessEvent(Event& e);
    void StartRoutine();

    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };

    static int num_arrival;

private:
    void PeerJoinEvent(Event& e);

    void PeerListReqRecvEvent(Event& e);

    void PeerListGetEvent(Event& e);

    void ReqPieceEvent(Event& e);

    void PieceAdmitEvent(Event& e);

    void PieceGetEvent(Event& e);

    void CompletedEvent(Event& e);

    void PeerLeaveEvent(Event& e);

    void MapEvent();
    void MapEventDeps();

    FuncMap event_map_;
    DepMap event_deps_map_;

    std::list<Event> event_list_;
    std::list<Event> system_;

    Args args_;

    const PeerManager* pm_;

    float lambda_;
    float mu_;

    float total_sys_size_;
    float current_time_;
    float waiting_time_;

    static int peer_join_counts;
};

#endif // for #ifndef _EVENT_HANDLER_H
