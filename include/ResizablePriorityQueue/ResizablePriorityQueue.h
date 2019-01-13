#pragma once
#include <stdint.h>
#include <queue>


using std::vector;
using std::priority_queue;


namespace ct {
  template <class _Ty,
    class _Pr = std::less<typename vector<_Ty>::value_type>>
  class ResizablePriorityQueue : public std::priority_queue<_Ty, vector<_Ty>, _Pr>
  {
  public:
    ResizablePriorityQueue(int32_t capacity = 0)
    {
      this->c.reserve(capacity);
    }

    int32_t GetContainerCapacity() const
    {
      return this->c.capacity();
    }

    bool SetCapacity(int32_t capacity)
    {
      if (this->c.capacity() > 0)
      {
        return false;
      }
      else
      {
        this->c.reserve(capacity);
        return true;
      }
    }
  };
}
