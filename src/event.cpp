#include "event.h"

Event::Event(Type t, Type4BT t_bt, int idx, int pid, float ti)
{
    type = t;
    type_bt = t_bt;
    index = idx;
    this->pid = pid;

    time = ti;
    pg_delay = 0.0;

    need_new_neighbors = false;

    time_req_send = 0.0;

    client_pid = -1;
    need_new_neighbors = false;
    is_complete = false;
    //req_timeout = false;
}

bool Event::operator<(const Event& e)
{
    return time < e.time;
}
