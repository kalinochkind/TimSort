enum EWhatMerge
{
    WM_NoMerge, WM_MergeXY, WM_MergeYZ
};

struct ITimSortParams
{
    virtual ~ITimSortParams() {};

    virtual size_t minRun(size_t n) const = 0;

    virtual bool needMerge(size_t lenX, size_t lenY) const = 0;

    virtual EWhatMerge whatMerge(size_t lenX, size_t lenY, size_t lenZ) const = 0;

    virtual size_t GetGallop() const = 0;
};

struct DefaultTimSortParams: public ITimSortParams
{
    size_t minRun(size_t n) const
    {
        return 12; // sure
    }

    bool needMerge(size_t lenX, size_t lenY) const
    {
        return lenX <= lenY;
    }

    EWhatMerge whatMerge(size_t lenX, size_t lenY, size_t lenZ) const
    {
        if (lenX <= lenY + lenZ)
        {
            if (lenX <= lenZ)
            {
                return WM_MergeXY;
            }
            else
            {
                return WM_MergeYZ;
            }
        }
        else if (lenY <= lenZ)
        {
            return WM_MergeYZ;
        }
        return WM_NoMerge;
    }

    size_t GetGallop() const
    {
        //return rand();
        return 42;
    }
};