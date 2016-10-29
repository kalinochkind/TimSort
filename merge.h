#ifndef __MERGE_H
#define __MERGE_H

#include <cmath>

namespace timsort
{
template<class RAI, class VAL, class Compare>
size_t Gallop(RAI begin, RAI end, const VAL &val, Compare comp)
{
    size_t r = 1;
    if (end - begin < 4)
        return 0;
    while (comp(*(begin + r), val))
    {
        r <<= 1;
        if (begin + r >= end)
        {
            r = end - begin;
            break;
        }
    }
    size_t l = 0;
    while (l + 1 < r)
    {
        size_t m = (l + r) >> 1;
        if (comp(*(begin + m), val))
        {
            l = m;
        }
        else
        {
            r = m;
        }
    }
    return l;
}

template<class RAI, class VAL, class Compare>
size_t GallopRev(RAI begin, RAI end, const VAL &val, Compare comp)
{
    size_t r = 1;
    while (comp(val, *(begin - r)))
    {
        r <<= 1;
        if (begin - r < end)
        {
            r = begin - end;
            break;
        }
    }
    size_t l = 0;
    while (l + 1 < r)
    {
        size_t m = (l + r) >> 1;
        if (comp(val, *(begin - m)))
        {
            l = m;
        }
        else
        {
            r = m;
        }
    }
    return l;
}


// This slow piece of poo is never called if you have enough memory
template<class RAI, class Compare>
void SwapMerge(RAI begin, RAI middle, RAI end, RAI buf_begin, Compare comp, size_t gallop)
{
    bool backward = (end - middle) < (middle - begin);
    SwapRange(backward ? middle : begin, backward ? end : middle, buf_begin);
    size_t cnta = 0, cntb = 0;
    if (backward)
    {
        RAI i1 = middle - 1, dest = end - 1, i2 = buf_begin + (end - middle - 1);
        while (i1 != begin - 1 && i2 != buf_begin - 1)
        {
            if (cnta > gallop)
            {
                size_t g = GallopRev(i1, begin - 1, *i2, comp);
                for (; g; --g, --i1, --dest)
                {
                    if (!comp(*i2, *i1) || i1 == begin - 1)
                        return;
                    Swap(*i1, *dest);
                }
                cnta = 0;
            }
            if (cntb > gallop)
            {
                size_t g = GallopRev(i2, buf_begin - 1, *i1, comp);
                for (; g; --g, --i2, --dest)
                {
                    if (comp(*i2, *i1) || i2 == buf_begin - 1)
                        return;
                    Swap(*i2, *dest);
                }
                cntb = 0;
            }
            if (comp(*i2, *i1))
            {
                Swap(*i1, *dest);
                --i1;
                cnta++;
                cntb = 0;
            }
            else
            {
                Swap(*i2, *dest);
                --i2;
                cntb++;
                cnta = 0;
            }
            --dest;
        }
        while (i2 != buf_begin - 1)
        {
            Swap(*i2, *dest);
            --dest;
            --i2;
        }
    }
    else
    {
        RAI i1 = buf_begin, dest = begin, i2 = middle;
        RAI buf_end = buf_begin + (middle - begin);
        while (i1 != buf_end && i2 != end)
        {
            if (cnta > gallop)
            {
                size_t g = Gallop(i1, buf_end, *i2, comp);
                for (; g; --g, ++i1, ++dest)
                {
                    if (!comp(*i1, *i2) || i1 == buf_end)
                        return;
                    Swap(*i1, *dest);
                }
                cnta = 0;
            }
            if (cntb > gallop)
            {
                size_t g = Gallop(i2, end, *i1, comp);
                for (; g; --g, ++i2, ++dest)
                {
                    if (comp(*i1, *i2) || i2 == end)
                        return;
                    Swap(*i2, *dest);
                }
                cntb = 0;
            }
            if (comp(*i1, *i2))
            {
                Swap(*i1, *dest);
                ++i1;
                cnta++;
                cntb = 0;
            }
            else
            {
                Swap(*i2, *dest);
                ++i2;
                cntb++;
                cnta = 0;
            }
            ++dest;
        }
        while (i1 != buf_end)
        {
            Swap(*i1, *dest);
            ++dest;
            ++i1;
        }
    }
}


template<class RAI, class BUF_RAI, class Compare>
void FastMerge(RAI begin, RAI middle, RAI end, BUF_RAI buf_begin, Compare comp)
{
    BUF_RAI buf_end = buf_begin;
    for (RAI i = begin; i != middle; ++i, ++buf_end)
    {
        *buf_end = *i;
    }
    while (buf_begin != buf_end && middle != end)
    {
        if (comp(*middle, *buf_begin))
        {
            *begin = *middle;
            ++middle;
        }
        else
        {
            *begin = *buf_begin;
            ++buf_begin;
        }
        ++begin;
    }
    while (buf_begin != buf_end)
    {
        *begin = *buf_begin;
        ++buf_begin;
        ++begin;
    }

}

template<class RAI, class Compare>
void InplaceMerge(RAI begin, RAI middle, RAI end, void *buf, Compare comp, size_t gallop)
{
    typedef typename std::iterator_traits<RAI>::value_type VAL;

    if (buf)
    {
        // Why not?
        FastMerge(begin, middle, end, reinterpret_cast<VAL *>(buf), comp);
        return;
    }
    if (end - begin < 64)
    {
        for (RAI i = begin + 1; i != end; ++i)
        {
            Insertion(begin, i, comp);
        }
        return;
    }
    size_t len = end - begin;
    size_t block_size = int(sqrt(len));
    size_t block_count = len / block_size;
    size_t mid_block = (middle - begin) / block_size;
    if (mid_block < block_count)
        SwapRange(begin + mid_block * block_size, begin + (mid_block + 1) * block_size,
                  begin + block_size * (block_count - 1));
    for (size_t i = 0; i < block_count - 1; ++i)
    {
        size_t mn = i;
        for (size_t j = i + 1; j < block_count - 1; ++j)
        {
            if (comp(begin[j * block_size], begin[mn * block_size]))
                mn = j;
            else if (!comp(begin[mn * block_size], begin[j * block_size]) &&
                     comp(begin[(j + 1) * block_size - 1], begin[(mn + 1) * block_size - 1]))
                mn = j;
        }
        SwapRange(begin + mn * block_size, begin + (mn + 1) * block_size, begin + i * block_size);
    }
    for (size_t i = 1; i < block_count - 1; ++i)
    {
        SwapMerge(begin + (i - 1) * block_size, begin + i * block_size, begin + (i + 1) * block_size,
                  begin + (block_count - 1) * block_size, comp, gallop);
    }
    size_t bs = (block_count - 1) * block_size - (len - (block_count - 1) * block_size);
    for (size_t i = bs + 1; i < len; ++i)
    {
        Insertion(begin + bs, begin + i, comp);
    }
    SwapMerge(begin, begin + bs, begin + (block_count - 1) * block_size, begin + (block_count - 1) * block_size, comp,
              gallop);
    for (size_t i = (block_count - 1) * block_size; i < len; ++i)
    {
        Insertion(begin + (block_count - 1) * block_size, begin + i, comp);
    }
}
}
#endif
