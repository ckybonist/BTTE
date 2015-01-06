#ifndef _RANDOM_IMPL_H
#define _RANDOM_IMPL_H

#include <cassert>

using namespace btte_uniformrand;  // for Rand(), Roll()


template<typename T>
T Roll(const RSC rsc,
       const T low,
       const T up)
{
    assert(up > low);

    T number = 0;

    typedef T type_T;  // define template parameter to real type

    if (typeid(int) == typeid(type_T))
    {
        // [low, up]
        double sub_formula_01 = ((double)Rand(rsc) / ((double)RAND_MAX + 1));

        double sub_formula_02 = sub_formula_01 * (up - low + 1);

        number = static_cast<T>(low + sub_formula_02);
    }
    else if (typeid(float) == typeid(type_T) || typeid(double) == typeid(type_T))
    {
        number = low + static_cast<T>(Rand(rsc)) / (static_cast<T>((double)RAND_MAX / (up - low)));
    }

    // [low, up)
	// int number = (int)(((double)Rand(rsc) / ((double)RAND_MAX + 1)) * (up - low)) + low;

	return number;
}

template<typename T>
T* DistinctRandNum(const RSC rsc,
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
void Shuffle(const RSC rsc, T *arr, const size_t N)
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
        std::size_t idx = Roll<T>(rsc, 0, N - 1);
        T temp = arr[idx];
        arr[idx] = arr[i];
        arr[i] = temp;
    }
}

template <class RandomIt>
//void RandomShuffle(RandomIt first, RandomIt last)
void RandomShuffle(const RSC rsc, RandomIt first, RandomIt last)
{
     typename std::iterator_traits<RandomIt>::difference_type i, n;
     n = last - first;
     for (i = n-1; i > 0; --i)
     {
         using std::swap;
         //swap(first[i], first[std::rand() % (i+1)]);
         swap(first[i], first[Rand(rsc) % (i+1)]);
     }
}


template<typename T>
T RangeRandNumExceptEx(const RSC rsc,
                       const size_t set_size,
                       const T* exclude_set)
{
	int target = 0;
	bool flag = true;
    const int low = 1;  // NOTE: don't use 0, it will conflict with loop counter
    const int up = set_size;

	while (flag)
    {
		target = Roll<int>(rsc, low, up);

		for (size_t i = 0; i < set_size; i++)
        {
			//if(target == ex_set[i] && ex_set[i] != 0)
			if (target == exclude_set[i])
            {
				flag = true;
				break;
			}
            else
            {
				flag = false;
			}
		}
	}

	return target;
}

template<template<typename, typename> class C_t, typename V_t>
V_t RandChooseElementInContainer(const RSC rsc, C_t<V_t, std::allocator<V_t>> const& cont)
{
    const size_t kSize = cont.size();

    if (kSize == 0)
        ExitError("In RandChooseElementInContainer() :Size of container must greater than zero\n");

    const int rand_pos = Rand(rsc) % kSize;
    auto it = cont.begin();
    for (int i = 0; i < rand_pos; ++i, ++it);
    return *it;
}

template <typename T>
T RandChooseElementInSet(const RSC rsc, std::set<T> myset)
{
    const int rand_pos = Rand(rsc) % myset.size();
    auto it = myset.begin();
    for (int i = 0; i < rand_pos; ++i, ++it);
    return *it;
}

#endif // for #ifndef _RANDOM_IMPL_H
