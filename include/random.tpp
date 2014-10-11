#ifndef _RANDOM_IMPL_H
#define _RANDOM_IMPL_H

#include <iostream>
#include <cassert>

#include "random.h"


template<typename T>
T Roll(const RSC& rsc,
       const T low,
       const T up)
{
    assert(up > low);
    // [low, up]
	T number = static_cast<T>((((double)uniformrand::Rand(rsc) / ((double)RAND_MAX + 1)) *
			     (up - low + 1)) + low);
    // [low, up)
	// int number = (int)(((double)uniformdist::rand(rsc) / ((double)RAND_MAX + 1)) *
			     //(up - low)) + low;
	return number;
}

template<typename T>
T* DistinctRandNum(const RSC& rsc,
                   const size_t size,
                   const T rand_limit)
{
    T* arr = new T[size];
    if(arr == nullptr)
        ExitError("\nMemory allocation failed!");

    for (size_t m = 0; m < size; m++)
    {
        T rand_num = Rand(rsc) % rand_limit;

        size_t s = m;

        bool duplicate = true;

        while (duplicate && s > 0)
        {
            if (rand_num == arr[s])  // rand_num is duplicate
            {
                rand_num = Rand(rsc) % rand_limit;
                s = m;
                continue;
            }
            else if (s == 0)  // rand_num is distinct
            {
                duplicate = false;
            }

            --s;
        }

        arr[m] = rand_num;
    }

    return arr;
}

template<typename T>
void Shuffle(const RSC& rsc, T *arr, size_t N)
{
    if (arr == nullptr)
    {
        ExitError("\nPassing nullptr as an array\n");
    }

    if (N < 1)
    {
        ExitError("\nSize must greater than 1\n");
    }

    for (std::size_t i = 0; i < N; i++)
    {
        std::size_t idx = uniformrand::Roll<T>(rsc, 0, N - 1);
        T temp = arr[idx];
        arr[idx] = arr[i];
        arr[i] = temp;
    }
}

#endif // for #ifndef _RANDOM_IMPL_H
