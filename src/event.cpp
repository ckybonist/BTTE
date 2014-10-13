#include "event.h"

Event::Event(Type t, Type4BT t_bt, int idx, int pid, float ti)
{
    type = t;
    type_bt = t_bt;
    index = idx;
    this->pid = pid;
    time = ti;
}

bool Event::operator<(const Event& e)
{
    if (time < e.time)
        return true;
    else
        return false;
    //return time < e.time;
}
