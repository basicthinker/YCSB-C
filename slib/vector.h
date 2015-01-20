//
//  vector.h
//  vm-persistence
//
//  Created by Jinglei Ren on Jan 18 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_SLIB_VECTOR_H_
#define VM_PERSISTENCE_SLIB_VECTOR_H_

#include <cassert>
#include "slib/mem_alloc.h"

namespace slib {

template <class T, class Alloc = MemAlloc>
class vector {
 public:
  vector(std::size_t n = 0);
  vector(const vector<T, Alloc> &other);
  vector<T, Alloc> &operator=(const vector<T, Alloc> &other);

  std::size_t size() const { return size_; }
  std::size_t capacity() const { return capacity_; }
  void resize(std::size_t n);

  T &operator[](std::size_t n) { assert(n < size_); return array_[n]; }
  void push_back(const T &val) __attribute__((transaction_safe));

  T *begin() { return array_; }
  T *end() { return array_ + size_; }

  ~vector();

 private:
  void set_capacity(std::size_t n) __attribute__((transaction_safe));

  int *counter_;
  T *array_;
  std::size_t capacity_;
  std::size_t size_;
};

template <class T, class Alloc>
void vector<T, Alloc>::set_capacity(std::size_t n) {
  array_ = Alloc::Realloc(array_, capacity_ * sizeof(T), n * sizeof(T));
  capacity_ = n;
}

template <class T, class Alloc>
vector<T, Alloc>::vector(std::size_t n) : array_(nullptr), size_(0) {
  counter_ = Alloc::template New<int>(1);
  set_capacity(n);
}

template <class T, class Alloc>
vector<T, Alloc>::vector(const vector<T, Alloc> &other) :
    counter_(other.counter_), array_(other.array_),
    capacity_(other.capacity_), size_(other.size_) {
  *counter_ += 1; 
}

template <class T, class Alloc>
vector<T, Alloc> &vector<T, Alloc>::operator=(const vector<T, Alloc> &other) {
  *other.counter_ += 1;
  this->~vector<T, Alloc>();
  counter_ = other.counter_;
  array_ = other.array_;
  capacity_ = other.capacity_;
  size_ = other.size_;
  return *this;
}
 
template <class T, class Alloc>
void vector<T, Alloc>::push_back(const T &val) {
  if (size_ == capacity_) {
    set_capacity(capacity_ * 2);
  }
  ::new(array_ + size_) T(val);
  ++size_;
}

template <class T, class Alloc>
vector<T, Alloc>::~vector() {
  *counter_ -= 1;
  if (*counter_) return;
  Alloc::Delete(counter_);
  for (T *p = array_; p < array_ + size_; ++p) {
    p->~T();
  }
  Alloc::Free(array_, capacity_ * sizeof(T));
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_VECTOR_H_

