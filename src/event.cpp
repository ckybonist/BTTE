#include "event.h"

Event::Event(Type t, Type4BT t_bt, int idx, float ti)
{
    type = t;
    type_bt = t_bt;
    index = idx;
    time = ti;
}

bool Event::operator< (const Event& e)
{
    return time < e.time;
}
