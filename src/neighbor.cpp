#include "neighbor.h"

Neighbor::Neighbor()
{
    id = -1;
    pg_delay = 0.0;
    connected = false;
}

Neighbor::Neighbor(const int pid,
                   const float pg_delay)
{
    id = pid;
    this->pg_delay = pg_delay;
    connected = true;
}
