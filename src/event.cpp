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
    return time < e.time;
}
