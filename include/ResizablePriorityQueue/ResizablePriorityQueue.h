#pragma once
#include <stdint.h>
#include <iostream>
#include <queue>


using std::vector;
using std::priority_queue;


namespace ct {
  template <class _Ty,
    class _Container = vector<_Ty>,
    class _Pr = less<typename _Container::value_type>>
  class ResizablePriorityQueue : public std::priority_queue<_Ty, _Container, _Pr> {
  public:
    ResizablePriorityQueue(int32_t capacity = 0) {
      this->c.reserve(capacity);
    }

    int32_t getContainerSize() const {
      return this->c.size();
    }
  };
}
