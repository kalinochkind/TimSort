#include <stack>
#include <cstdlib>
#include "timsort_params.h"
#include "utils.h"
#include "merge.h"

template<class PAIR, class Compare>
void validateStack(std::stack<PAIR> &stack, void *buf, Compare comp, const ITimSortParams &params)
{
    if (stack.size() < 2)
        return;
    PAIR z = stack.top();
    stack.pop();
    PAIR y = stack.top();
    stack.pop();
    if (stack.size() >= 3)
    {
        PAIR x = stack.top();
        stack.pop();
        EWhatMerge wm = params.whatMerge(x.second, y.second, z.second);
        if (wm == WM_MergeXY)
        {
            InplaceMerge(x.first, y.first, y.first + y.second, buf, comp, params.GetGallop());
            x.second += y.second;
            stack.push(x);
            stack.push(z);
            validateStack(stack, buf, comp, params);
        }
        else if (wm == WM_MergeYZ)
        {
            InplaceMerge(y.first, z.first, z.first + z.second, buf, comp, params.GetGallop());
            y.second += z.second;
            stack.push(x);
            stack.push(y);
            validateStack(stack, buf, comp, params);
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
        validateStack(stack, buf, comp, params);
        return;
    }
    stack.push(y);
    stack.push(z);
}

template<class RAI>
void TimSort(RAI first, RAI last)
{
    TimSort(first, last, Less<typename std::iterator_traits<RAI>::value_type>(), DefaultTimSortParams());
}

template<class RAI, class Compare>
void TimSort(RAI first, RAI last, Compare comp)
{
    TimSort(first, last, comp, DefaultTimSortParams());
};

template<class RAI, class Compare>
void TimSort(RAI first, RAI last, const Compare &comp, const ITimSortParams &params)
{
    typedef typename std::iterator_traits<RAI>::value_type VAL;
    std::stack<std::pair<RAI, size_t> > stack;
    VAL *buf;
    try
    {
        buf = new VAL[last - first];
        //buf = 0;
    }
    catch (std::bad_alloc)
    {
        buf = nullptr;
    }
    int minRun = params.minRun(last - first);
    int runLen = 1;
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
            Reverse(i - runLen, i);
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
        Insertion(i - runLen, i, comp);
        lastValue = *i;
    }
    if (descending)
    {
        Reverse(last - runLen, last);
    }
    stack.push({last - runLen, runLen});
    while (stack.size() > 1)
    {
        auto a = stack.top();
        stack.pop();
        auto b = stack.top();
        stack.pop();
        InplaceMerge(b.first, a.first, a.first + a.second, buf, comp, params.GetGallop());
        b.second += a.second;
        stack.push(b);
    }
    if (buf)
        delete[] buf;
    return;
};
