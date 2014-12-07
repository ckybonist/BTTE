#ifndef _ALGO_H
#define _ALGO_H


/* Reference to C++ <algorithm> set_difference method */
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator btte_set_diff(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             OutputIterator result);

#include "general_algo.tpp"

#endif // for #ifndef _ALGO_H
