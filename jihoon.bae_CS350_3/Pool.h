#pragma once

#include <queue>
#include <set>

template <typename T>
class Pool 
{
public:
    Pool()
    {
    }

    Pool(const Pool& val) : pool_(val.pool_),
                            borrowed_(val.borrowed_)
    {
    }

    ~Pool()
    {
    }

    void Push(const T& val)
    {
        pool_.push(val);
    }

    T Pop()
    {
        T ret(pool_.front());
        pool_.pop();
        borrowed_.insert(ret);
        return ret;
    }

    void Return(const T& val)
    {
        std::multiset<T>::iterator it = borrowed_.find(val);
        pool_.push(*it);
        borrowed_.erase(it);
    }
private:
    std::queue<T> pool_;
    std::multiset<T> borrowed_;
};
