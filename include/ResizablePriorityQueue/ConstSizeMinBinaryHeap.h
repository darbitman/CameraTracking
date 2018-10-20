#pragma once


template<typename _Tp>
class ConstSizeMinBinaryHeap {
public:
  /**
   * @brief initialize min oriented binary heap
   * @param capacity maximum number of elements
   */
  explicit ConstSizeMinBinaryHeap(uint32_t capacity);

  /**
   * @brief default constructor to initialize to null values
   */
  ConstSizeMinBinaryHeap();

  /**
   * @brief deallocate memory
   */
  ~ConstSizeMinBinaryHeap();

  /**
   * @brief copy constructor to perform deep copy
   * @param rhs source of deep copy
   */
  ConstSizeMinBinaryHeap(ConstSizeMinBinaryHeap<_Tp>& rhs);

  /**
   * @brief initialize data members and allocate memory for new heap
   * @param capacity maximum number of elements
   * @return returns false if could not allocate memory, otherwise return true once memory is allocated
   */
  bool allocate(uint32_t capacity);

  /**
   * @brief insert new element
   * @param element element to insert
   */
  bool push(_Tp element);

  /**
   * @brief delete minimum element and return it
   * @return minimum element
   */
  _Tp deleteMin();

  /**
   * @brief return minimum element without deleting it
   * @return minimum element
   */
  _Tp getMin() const;

  /**
   * @brief return the number of elements in the queue
   */
  uint32_t size() const;

  /**
   * @brief check if the queue is empty
   * @return bool returns true if queue is empty
   */
  bool empty() const;

protected:
  /**
   * @brief promote element k if less than its parent
   * @param k index of element to promote
   */
  void swim(uint32_t k);

  /**
   * @brief demote element k if greater than its parent
   * @param k index of element to demote
   */
  void sink(uint32_t k);

  /**
   * @brief swap 2 elements
   * @param j index of the first element
   * @param k index of the second element
   */
  void exch(uint32_t j, uint32_t k);

  // max number of elements
  uint32_t capacity_;

  // position of last element
  // also the number of elements. returned when size is called
  uint32_t N_;

  // pointer to where the raw data will be stored
  _Tp* heap_;
};


template<typename _Tp>
ConstSizeMinBinaryHeap<_Tp>::ConstSizeMinBinaryHeap(uint32_t capacity) : N_(0), capacity_(capacity), heap_(nullptr) {
  // make sure allocating a heap of at least 1 element
  if (capacity_ > 0) {
    // binary heap functions do not use element 0
    // so need an extra element in array
    heap_ = new _Tp[capacity_ + 1];
  }
}


template<typename _Tp>
ConstSizeMinBinaryHeap<_Tp>::ConstSizeMinBinaryHeap(ConstSizeMinBinaryHeap<_Tp>& rhs) {
  N_ = rhs.N_;
  capacity_ = rhs.capacity_;
  heap_ = nullptr;
  // make sure allocating a heap of at least 1 element
  if (capacity_ > 0) {
    heap_ = new _Tp[capacity_ + 1];
    for (uint32_t i = 1; i < N_ + 1; i++) {
      heap_[i] = rhs.heap_[i];
    }
  }
}

template<typename _Tp>
bool ConstSizeMinBinaryHeap<_Tp>::allocate(uint32_t capacity) {
  // check if heap has already been allocated to prevent memory leaks
  // make sure allocating a heap of at least 1 element
  if (heap_ == nullptr && capacity > 0) {
    N_ = 0;
    capacity_ = capacity;
    heap_ = new _Tp[capacity_ + 1];
    return true;
  }
  else {
    return false;
  }
}


template<typename _Tp>
bool ConstSizeMinBinaryHeap<_Tp>::push(_Tp element) {

  // verify memory exists
  // verify that capacity is non-zero and positive
  if (heap_ != nullptr && capacity_ > 0) {
    heap_[++N_] = element;
    swim(N_);
  }
}


template<typename _Tp>
_Tp ConstSizeMinBinaryHeap<_Tp>::deleteMin() {
  // initialize default return value
  _Tp min = _Tp();
  // verify memory exists
  if (heap_ != nullptr) {
    // save root element
    min = heap_[1];
    // swap root element and last element
    exch(1, N_--);
    // demote root to reorder heap
    sink(1);
  }
  return min;
}


template<typename _Tp>
_Tp ConstSizeMinBinaryHeap<_Tp>::getMin() const {
  _Tp min = _Tp();
  if (heap_ != nullptr) {
    min = heap_[1];
  }
  return min;
}


template<typename _Tp>
uint32_t ConstSizeMinBinaryHeap<_Tp>::size() const {
  return this->N_;
}


template<typename _Tp>
bool ConstSizeMinBinaryHeap<_Tp>::empty() const {
  return this->N_ == 0;
}


template<typename _Tp>
ConstSizeMinBinaryHeap<_Tp>::ConstSizeMinBinaryHeap() {
  this->N_ = 0;
  this->capacity_ = 0;
  this->heap_ = nullptr;
}


template<typename _Tp>
ConstSizeMinBinaryHeap<_Tp>::~ConstSizeMinBinaryHeap() {
  delete[] heap_;
}


template<typename _Tp>
void ConstSizeMinBinaryHeap<_Tp>::swim(uint32_t k) {
  // check if we're not at root node and if child is less than parent
  // if so, swap the elements
  while (k > 1 && (heap_[k] < heap_[k / 2])) {
    exch(k, k / 2);
    k = k / 2;
  }
}


template<typename _Tp>
void ConstSizeMinBinaryHeap<_Tp>::sink(uint32_t k) {
  while (2 * k <= N_) {
    uint32_t j = 2 * k;
    // check if left child is greater than right child
    // if so, increment j to point to right child
    if (j < N_ && (heap_[j] > heap_[j + 1])) {
      j++;
    }
    // if parent is less than the smallest child, don't need to do anything
    if (heap_[k] < heap_[j]) {
      break;
    }
    // swap parent and smaller child
    exch(k, j);
    k = j;
  }
}


template<typename _Tp>
void ConstSizeMinBinaryHeap<_Tp>::exch(uint32_t j, uint32_t k) {
  _Tp swap = heap_[j];
  heap_[j] = heap_[k];
  heap_[k] = swap;
}
