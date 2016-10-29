template<class T>
class Stack
{
    T *arr;
    size_t _size;
public:
    explicit Stack(size_t sz): _size(0)
    {
        arr = new T[sz];
    }

    Stack(Stack<T> &a) = delete;

    void operator=(Stack<T> &a) = delete;

    ~Stack()
    {
        delete[] arr;
    }

    void push(const T &a)
    {
        arr[_size++] = a;
    }

    const T pop()
    {
        return arr[--_size];
    }

    size_t size() const
    {
        return _size;
    }

};