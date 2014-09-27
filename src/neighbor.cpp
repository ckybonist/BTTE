#include <iostream>

#include "neighbor.h"

Neighbor::Neighbor(const int k_id, const int k_pg_delay) {
    this->id = k_id;
    this->pg_delay = k_pg_delay;
}
