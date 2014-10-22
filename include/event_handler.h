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

    void PushInitEvent();
    void ProcessArrival(Event& e);
    void ProcessDeparture(Event& e);
    void ProcessEvent(Event& e);
    void StartRoutine();

    float get_lambda() { return lambda_; };
    float get_mu() { return mu_; };


private:
    void GetNextArrivalEvent(const Event::Type4BT next_tbt,
                             const int next_index,
                             const int next_pid,
                             const float next_etime);

    void GetNextDepartureEvent(const Event::Type4BT orig_tbt,
                               const int next_index,
                               const int orig_pid);

    float GetNextArrivalEventTime(const Event::Type4BT t_bt,
                                  float time_packet,
                                  const float current_arrival_etime);

    float GetNextDepartureEventTime();


    void GetNextDepartureEvent(const Event &);

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

    static int next_event_idx_;

    // count hom many peer-join-event was generated
    static int peer_join_counts_;

    typedef void (EventHandler::*Fptr)(Event&);
    typedef std::map<Event::Type4BT, Fptr> FuncMap;
    FuncMap event_map_;
    std::map<Event::Type4BT, Event::Type4BT> event_deps_map_;

    std::list<Event> event_list_;
    std::list<Event> system_;

    Args args_;

    PeerManager* pm_;

    float lambda_;
    float mu_;

    static const float kTimeout_;

    float total_sys_size_;
    float current_time_;
    float waiting_time_;
};

#endif // for #ifndef _EVENT_HANDLER_H
