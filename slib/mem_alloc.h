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
#include <iostream>
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
};

struct SvmAlloc {
  static void *Malloc(std::size_t size) {
    return sitevm::smalloc(size);
  }

  template <typename T>
  static void Free(T *p, std::size_t size) {
    //__transaction_atomic {
    //memset((void *)p, 255, size);
      sitevm::sfree((void *)p);
      //}
  }

  template <typename T, typename... Arguments>
  static T *New(Arguments... args) {
    __transaction_atomic {
      void *p = Malloc(sizeof(T));
      ::new(p) T(args...);
      return (T *)p;
    }
  }

  template <typename T> __attribute__((transaction_pure))
  static void Delete(T *p) {
    __transaction_atomic {
      p->~T();
      Free(p, sizeof(T));
    }
  }
};

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_MEM_ALLOC_H_

