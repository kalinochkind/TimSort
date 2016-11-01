#ifndef __TIMSORT_H
#define __TIMSORT_H

#include <cstdlib>
#include <iterator>
#include "stack.h"
#include "timsort_params.h"
#include "utils.h"
#include "merge.h"

namespace timsort
{
template<class PAIR, class Compare>
void validateStack(Stack<PAIR> &stack, void *buf, Compare comp, const ITimSortParams &params)
{
    validate_stack_begin:
    if (stack.size() < 2)
        return;
    PAIR z = stack.pop();
    PAIR y = stack.pop();
    if (stack.size() >= 3)
    {
        PAIR x = stack.pop();
        EWhatMerge wm = params.whatMerge(x.second, y.second, z.second);
        if (wm == WM_MergeXY)
        {
            InplaceMerge(x.first, y.first, y.first + y.second, buf, comp, params.GetGallop());
            x.second += y.second;
            stack.push(x);
            stack.push(z);
            goto validate_stack_begin;
        }
        else if (wm == WM_MergeYZ)
        {
            InplaceMerge(y.first, z.first, z.first + z.second, buf, comp, params.GetGallop());
            y.second += z.second;
            stack.push(x);
            stack.push(y);
            goto validate_stack_begin;
        }
        else
        {
            stack.push(x);
            stack.push(y);
            stack.push(z);
        }
        return;
    }
    if (params.needMerge(y.second, z.second))
    {
        InplaceMerge(y.first, z.first, z.first + z.second, buf, comp, params.GetGallop());
        y.second += z.second;
        stack.push(y);
        goto validate_stack_begin;
        return;
    }
    stack.push(y);
    stack.push(z);
}
}

template<class RAI>
void TimSort(RAI first, RAI last)
{
    TimSort(first, last, timsort::Less<typename std::iterator_traits<RAI>::value_type>(), DefaultTimSortParams());
}

template<class RAI, class Compare>
void TimSort(RAI first, RAI last, Compare comp)
{
    TimSort(first, last, comp, DefaultTimSortParams());
}

template<class RAI, class Compare>
void TimSort(RAI first, RAI last, const Compare &comp, const ITimSortParams &params)
{
    typedef typename std::iterator_traits<RAI>::value_type VAL;
    timsort::Stack<timsort::Pair<RAI, size_t> > stack(int(3 * log(last - first)) + 5);
    VAL *buf;
#ifdef FORCE_INPLACE_MERGE
    buf = nullptr;
#else
    try
    {
        buf = new VAL[last - first];
        //buf = 0;
    }
    catch (std::bad_alloc)
    {
        buf = nullptr;
    }
#endif
    size_t minRun = params.minRun(last - first);
    size_t runLen = 1;
    bool descending = false;
    VAL lastValue = *first;
    for (RAI i = first + 1; i < last; ++i, ++runLen)
    {
        bool ge = !comp(*i, lastValue);
        if ((ge ^ descending) || runLen == 1)
        {
            descending = !ge;
            lastValue = *i;
            continue;
        }
        if (descending)
        {
            timsort::Reverse(i - runLen, i);
            descending = false;
        }
        if (runLen >= minRun)
        {
            stack.push({i - runLen, runLen});
            validateStack(stack, buf, comp, params);
            runLen = 0;
            lastValue = *i;
            descending = false;
            continue;
        }
        timsort::Insertion(i - runLen, i, comp);
        lastValue = *i;
    }
    if (descending)
    {
        timsort::Reverse(last - runLen, last);
    }
    stack.push({last - runLen, runLen});
    while (stack.size() > 1)
    {
        timsort::Pair<RAI, size_t> a = stack.pop();
        timsort::Pair<RAI, size_t> b = stack.pop();
        timsort::InplaceMerge(b.first, a.first, a.first + a.second, buf, comp, params.GetGallop());
        b.second += a.second;
        stack.push(b);
    }
    if (buf)
        delete[] buf;
    return;
}

#endif
