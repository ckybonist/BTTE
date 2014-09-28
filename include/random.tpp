#ifndef _RANDOM_IMPL_H
#define _RANDOM_IMPL_H

#include <cassert>

#include "random.h"

template<typename T>
T Roll(const RSC& k_seed_rsc_id,
       const T k_low,
       const T k_up)
{
    assert(k_up > k_low);
    // [low, up]
	T number = static_cast<T>((((double)uniformrand::Rand(k_seed_rsc_id) / ((double)g_k_rand_max + 1)) *
			     (k_up - k_low + 1)) + k_low);
    // [low, up)
	/* int number = (int)(((double)uniformdist::rand(k_seed_rsc_id) / ((double)g_k_rand_max + 1)) *
			     (k_up - k_low)) + k_low; */
	return number;
}

template<typename T>
T* DistinctRandNumbers(const RSC& k_seed_rsc_id,
                       size_t k_size,
                       const T k_rand_limit)
{
    T *arr = new T[k_size];

    for(size_t m = 0; m < k_size; m++)
    {
        T rand_num = Rand(k_seed_rsc_id) % k_rand_limit + 1;

        T s = m;

        bool duplicate = true;

        while(duplicate && s > 0)
        {
            --s;
            if(rand_num == arr[s])  // rand_num is duplicate
            {
                rand_num = Rand(k_seed_rsc_id) % k_rand_limit + 1;
                s = m;
                continue;
            }

            if(rand_num != arr[s] && s == 0)  // rand_num is distinct
            {
                duplicate = false;
            }
        }

        arr[m] = rand_num;
    }

    return arr;
}

template<typename T>
void Shuffle(const RSC& k_seed_rsc_id, T *arr, size_t N)
{
    if(arr == nullptr)
    {
        ExitError("\nPassing nullptr as an array\n");
    }

    if(N < 1)
    {
        ExitError("\n Size must greater than 1\n");
    }

    //for (std::size_t i = 0; i < N - 1; i++)
    for (std::size_t i = 0; i < N; i++)
    {
      //std::size_t idx = i + uniformrand::Rand(k_seed_rsc_id) / (g_k_rand_max / (N - i) + 1);
      std::size_t idx = uniformrand::Roll<T>(k_seed_rsc_id, 0, N - 1);
      T temp = arr[idx];
      arr[idx] = arr[i];
      arr[i] = temp;
    }
}

#endif // for #ifndef _RANDOM_IMPL_H
