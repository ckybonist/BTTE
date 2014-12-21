#include <cmath>

#include "number.h"

/*
 * Proposed D.E.Knuth in section 4.2.2 pages 217-218
 * of Seminumerical Algorithms
 *
 * */
bool IsFloatEqual(const double x, const double y)
{
    // some small number such as 1e-5
    const double epsilon = 1e-5;
    return std::abs(x - y) <= epsilon * std::abs(x);
}
