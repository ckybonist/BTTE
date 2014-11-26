#include "neighbor.h"

Neighbor::Neighbor()
{
    //id = -1;
    pg_delay = 0.0;
    //exist = false;
}

//Neighbor::Neighbor(const int pid,
//                   const float pg_delay)
//{
//    id = pid;
//    this->pg_delay = pg_delay;
//    exist = true;
//}

Neighbor::Neighbor(const float pg_delay)
{
    this->pg_delay = pg_delay;
}
