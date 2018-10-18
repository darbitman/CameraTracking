#pragma once
#include <stdint.h>

template<typename T>
class ConstSizeBinaryHeap {
public:
  // initialize data members and allocate memory for new heap
  void allocate(uint32_t capacity);
  
  // insert new element
  void insert(T x);

  // deletes root element and returns it
  T delRoot();

  // returns root element without deleting it
  T getRoot() const;

  // return capacity of heap
  uint32_t getSize() const;
protected:

  // store capacity of heap
  uint32_t capacity_;

  // store position of last element
  uint32_t N_;

  // heap stored as array
  T* heap_;

  // exchange elements at index j and index k
  void exch(uint32_t j, uint32_t k);

  // promote element k
  virtual void swim(uint32_t k) = 0;

  // demote element k
  virtual void sink(uint32_t k) = 0;
};


template<typename T>
void ConstSizeBinaryHeap<T>::allocate(uint32_t capacity) {
  // check if heap has already been allocated to prevent memory leaks
  // make sure allocating a heap of at least 1 element
  if (heap_ == nullptr && capacity > 0) {
    N_ = 0;
    capacity_ = capacity;
    heap_ = new T[capacity_ + 1];
  }
}


template<typename T>
void ConstSizeBinaryHeap<T>::insert(T x) {
  // verify memory exists
  // verify that capacity is non-zero and positive
  if (heap_ != nullptr && capacity_ > 0) {
    heap_[++N_] = x;
    swim(N_);
  }
}


// after root is deleted, heap order is restored
template<typename T>
T ConstSizeBinaryHeap<T>::delRoot() {
  // initialize default return value
  T root = T();
  // verify memory exists
  if (heap_ != nullptr) {
    // save root element
    root = heap_[1];
    // swap root element and last element
    exch(1, N_--);
    // demote root to reorder heap
    sink(1);
  }
  return root;
}


template<typename T>
T ConstSizeBinaryHeap<T>::getRoot() const {
  T root = T();
  if (heap_ != nullptr) {
    root = heap_[1];
  }
  return root;
}


template<typename T>
uint32_t ConstSizeBinaryHeap<T>::getSize() const {
  return N_;
}


template<typename T>
void ConstSizeBinaryHeap<T>::exch(uint32_t j, uint32_t k) {
  T swap = heap_[j];
  heap_[j] = heap_[k];
  heap_[k] = swap;
}
