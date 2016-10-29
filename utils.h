template<class RAI, class Compare>
void Insertion(RAI begin, RAI end, Compare comp)
{
    typedef typename std::iterator_traits<RAI>::value_type VAL;
    RAI l = begin - 1, r = end;
    while (r - l > 1)
    {
        RAI m = l + (r - l) / 2;
        if (comp(*end, *m))
            r = m;
        else
            l = m;
    }
    VAL val = *end;
    for (RAI c = end; c != r; --c)
    {
        *c = *(c - 1);
    }
    //memmove(&*(r + 1), &*r, (end - r) * sizeof(VAL));
    *r = val;
}

template<class T>
struct Less
{
    bool operator()(const T &a, const T &b) const
    {
        return a < b;
    }
};

template <class T1, class T2>
struct Pair
{
    T1 first;
    T2 second;
    Pair(const T1 &f, const T2 &s): first(f), second(s) {};
    Pair(){};
};

template<class T>
void Swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

template<class RAI>
void SwapRange(RAI begin, RAI end, RAI dest)
{
    for (; begin != end; ++begin, ++dest)
    {
        Swap(*begin, *dest);
    }
}

template<class RAI>
void Reverse(RAI begin, RAI end)
{
    --end;
    for (; begin < end; ++begin, --end)
    {
        Swap(*begin, *end);
    }
}