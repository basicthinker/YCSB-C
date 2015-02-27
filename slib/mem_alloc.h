//
//  mem_alloc.h
//  vm-persistence
//
//  Created by Jinglei Ren on 12/23/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_SLIB_MEM_ALLOC_H_
#define VM_PERSISTENCE_SLIB_MEM_ALLOC_H_

#include <cstring>
#include "sitevm/sitevm.h"
#include "slib/assert.h"

namespace slib {

struct MemAlloc {
  static void *Malloc(std::size_t size) { return malloc(size); }

  template <typename T> __attribute__((transaction_safe))
  static T *Realloc(T *old_pos, std::size_t old_size, std::size_t new_size) {
    T *new_pos = (T *)Malloc(new_size);
    if (old_pos) {
      memcpy(new_pos, old_pos, old_size < new_size ? old_size : new_size);
      Free(old_pos, old_size);
    }
    return new_pos;
  }

  template <typename T>
  static void Free(T *p, std::size_t size) { free((void *)p); }

  template <typename T, typename... Arguments>
  static T *New(Arguments... args) { return new T(args...); }

  template <typename T>
  static void Delete(T *p) { return delete p; }

  static void ThreadInit() { }
  static void ThreadExit() { }
};

struct SvmAlloc {
  static void *Malloc(std::size_t size) {
    __transaction_atomic {
      return smalloc(size);
    }
  }

  template <typename T>
  static T *Realloc(T *old_pos, std::size_t old_size, std::size_t new_size) {
    __transaction_atomic {
      return (T *)srealloc((void *)old_pos, new_size);
    }
  }

  template <typename T>
  static void Free(T *p, std::size_t size) {
    __transaction_atomic {
      memset((void *)p, 255, size);
      sfree((void *)p);
    }
  }

  template <typename T, typename... Arguments>
  static T *New(Arguments... args) {
    __transaction_atomic {
      void *p = smalloc(sizeof(T));
      ::new(p) T(args...);
      return (T *)p;
    }
  }

  template <typename T> __attribute__((transaction_pure))
  static void Delete(T *p) {
    p->~T();
    Free(p, sizeof(T));
  }

  __attribute__((transaction_pure))
  static void ThreadInit() {
    int err = sitevm::sitevm_enter();
    assert(!err);
  }

  __attribute__((transaction_pure))
  static void ThreadExit() { }

private:
  __attribute__((transaction_pure))
  static void* smalloc(size_t size) {
    return sitevm::smalloc(size);
  }

  __attribute__((transaction_pure))
  static void* scalloc(size_t no, size_t size) {
    return sitevm::scalloc(no, size);
  }

  __attribute__((transaction_pure))
  static void* srealloc(void* mem, size_t size) {
    return sitevm::srealloc(mem, size);
  }

  __attribute__((transaction_pure))
  static void sfree(void *p) {
      sitevm::sfree((void *)p);
  }
};

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_MEM_ALLOC_H_

