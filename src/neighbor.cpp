#include <iostream>

#include "neighbor.h"

Neighbor::Neighbor(const int id, const int pg_delay) {
    this->id = id;
    this->pg_delay = pg_delay;
}
