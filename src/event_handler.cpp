#include <iostream>

#include "random.h"
#include "peer.h"
#include "piece.h"
#include "event_handler.h"

using namespace uniformrand;

int EventHandler::num_arrival = 10000;

EventHandler::EventHandler(Args args, const PeerManager* pm, float l, float m) { _args = args;
    _args = args;
    _pm = pm;
    _lambda = l;
    _mu = m;
    _current_time = 0.0;
    _waiting_time = 0.0;
}

void EventHandler::PushInitEvent()
{
    float time = ExpRand(_lambda, Rand(RSC::EVENT_TIME));
    const int pid = _args.NUM_SEED + _args.NUM_LEECH;
    Event first_event(Event::Type::ARRIVAL,
                      Event::Type4BT::PEER_JOIN,
                      0,
                      pid,
                      time);
    _event_list.push_back(first_event);
}

void EventHandler::GetNextEvent(Event& e, Event::Type t, Event::Type4BT t_bt)
{
    if (Event::Type::ARRIVAL == t)
    {
        float time = e.time + ExpRand(_lambda, Rand(RSC::EVENT_TIME));
        Event new_event(t, t_bt, e.index + 1, e.pid + 1, time);
        _event_list.push_back(new_event);
    }
    else if (Event::Type::DEPARTURE == t)
    {
        float time = _current_time + ExpRand(_mu, Rand(RSC::EVENT_TIME));
        Event depart_event(t, t_bt, e.index, e.pid, time);
        _event_list.push_back(depart_event);

    }
}

void EventHandler::ProcessArrival(Event& e)
{
    _total_sys_size += _system.size();

    _system.push_back(e);

    std::cout.precision(5);
    std::cout << "\nBT event type: " << static_cast<int>(e.type_bt) << "\n";
    std::cout << "\nPID of BT event: " << e.pid;
    std::cout << "\nEvent time " << e.time << "\n\n";

    // 處理 system 裡第一個 BT 事件的函式
    // TODO : 執行相對應的事件處理函式
    ProcessBTEvent(e);

    // 產生下一個衍生事件, 中止條件為所有 pieces 拿到
    //if(e.index < num_arrival &&
    //   e.type_bt != Event::Type4BT::PEER_LEAVE)
    if (!g_all_pieces_get)
    {
        Event::Type4BT derived_type = GetDeriveBTEventType(e.type_bt);
        GetNextEvent(e, Event::Type::ARRIVAL, derived_type);
    }

    if (_system.size() == 1)
    {
        _current_time = e.time;

        GetNextEvent(e, Event::Type::DEPARTURE, e.type_bt);
    }

    _event_list.sort();
}

void EventHandler::ProcessDeparture(Event& e)
{
    _system.pop_front();

    if (_system.size() != 0)
    {
        Event ev = _system.front();
        _current_time = e.time;
        _waiting_time = _waiting_time + (_current_time - ev.time);

        GetNextEvent(ev, Event::Type::DEPARTURE, e.type_bt);
    }

    _event_list.sort();
}

void EventHandler::ProcessEventPeerJoin(Event& e)
{
    const int cid = 0;  // not implemnet yet
    _pm->NewPeer(e.pid, cid, _current_time);
}

void EventHandler::ProcessEventPeerListReqRecv(Event& e)
{
    std::cout << "PID for selecting neighbors: " << e.pid << std::endl;
    _pm->AllotNeighbors(e.pid);
}

void EventHandler::ProcessEventPeerListGet(Event& e)
{

}

void EventHandler::ProcessEventReqPiece(Event& e)
{
    // for debug usage
    for(int c = 0; c < _args.NUM_PIECE; c++)
    {
        g_peers[e.pid].pieces[c] = true;
    }
}

void EventHandler::ProcessEventPieceAdmit(Event& e)
{

}

void EventHandler::ProcessEventPieceGet(Event& e)
{

}

void EventHandler::ProcessEventCompleted(Event& e)
{

}

void EventHandler::ProcessEventPeerLeave(const Event& e)
{

}

Event::Type4BT EventHandler::GetDeriveBTEventType(Event::Type4BT t_bt)
{
    // Event Dependencies
    switch (t_bt)
    {
        case Event::Type4BT::PEER_JOIN:
            return Event::Type4BT::PEER_LIST_REQ_RECV;
            break;

        case Event::Type4BT::PEER_LIST_REQ_RECV:
            return Event::Type4BT::PEER_LIST_GET;
            break;

        case Event::Type4BT::PEER_LIST_GET: //        ProcessEventPeerListGet(e);
            return Event::Type4BT::REQ_PIECE;
            break;

        case Event::Type4BT::REQ_PIECE:
            return Event::Type4BT::PIECE_ADMIT;
            break;

        case Event::Type4BT::PIECE_ADMIT:
            return Event::Type4BT::PIECE_GET;
            break;

        case Event::Type4BT::PIECE_GET:
            return Event::Type4BT::COMPLETED;
            break;

        case Event::Type4BT::COMPLETED:
            return Event::Type4BT::PEER_LEAVE;
            break;

        case Event::Type4BT::PEER_LEAVE:
            break;

        default:
            break;
    }
}

void EventHandler::ProcessBTEvent(Event &e)
{
    // Event Dependencies
    switch (e.type_bt)
    {
        case Event::Type4BT::PEER_JOIN:
            ProcessEventPeerJoin(e);
            break;

        case Event::Type4BT::PEER_LIST_REQ_RECV:
            ProcessEventPeerListReqRecv(e);
            break;

        case Event::Type4BT::PEER_LIST_GET: //        ProcessEventPeerListGet(e);
            ProcessEventPeerListGet(e);
            break;

        case Event::Type4BT::REQ_PIECE:
            ProcessEventReqPiece(e);
            break;

        case Event::Type4BT::PIECE_ADMIT:
            ProcessEventPieceAdmit(e);
            break;

        case Event::Type4BT::PIECE_GET:
            ProcessEventPieceGet(e);
            break;

        case Event::Type4BT::COMPLETED:
            ProcessEventCompleted(e);
            break;

        case Event::Type4BT::PEER_LEAVE:
            ProcessEventPeerLeave(e);
            break;

        default:
            break;
    }
}

void EventHandler::ProcessEvent(Event& e)
{
    if(e.type == Event::Type::ARRIVAL)
    {
        ProcessArrival(e);
    }
    else if(e.type == Event::Type::DEPARTURE)
    {
        ProcessDeparture(e);
    }

    if(e.pid < (_args.NUM_PEER - 1) - 1)
        GetNextEvent(e, Event::Type::ARRIVAL, Event::Type4BT::PEER_JOIN);

    _event_list.sort();
}

void EventHandler::StartRoutine()
{
    PushInitEvent();

    // FIXME: Infinite loop
    while(!_event_list.empty())
    {
        Event head = _event_list.front();

        _event_list.pop_front();

        ProcessEvent(head);
        //std::cout << "Infinite loop\n";
    }
}
