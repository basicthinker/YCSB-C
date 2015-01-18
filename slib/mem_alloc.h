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
#include "sitevm/sitevm_malloc.h"

struct MemAlloc {
  static void *Malloc(std::size_t size) { return malloc(size); }

  template <typename T>
  static T *Realloc(T *ptr, std::size_t size) {
    return (T *)realloc((void *)ptr, size);
  }

  template <typename T>
  static void Free(T *p, std::size_t size) { free((void *)p); }

  template <typename T, typename... Arguments>
  static T *New(Arguments... args) { return new T(args...); }

  template <typename T>
  static void Delete(T *p) { return delete p; }
};

struct SvmAlloc {
  __attribute__((transaction_pure))
  static void *Malloc(std::size_t size) { return sitevm_malloc::smalloc(size); }

  template <typename T> __attribute__((transaction_pure))
  static T *Realloc(T *ptr, std::size_t size) {
    return (T *)sitevm_malloc::srealloc((void *)ptr, size);
  }

  template <typename T> __attribute__((transaction_pure))
  static void Free(T *p, std::size_t size) {
    memset((void *)p, 255, size);
    sitevm_malloc::sfree((void *)p);
  }

  template <typename T, typename... Arguments> __attribute__((transaction_pure))
  static T *New(Arguments... args) {
    void *p = Malloc(sizeof(T));
    ::new(p) T(args...);
    return (T *)p;
  }

  template <typename T> __attribute__((transaction_pure))
  static void Delete(T *p) {
    p->~T();
    Free(p, sizeof(T));
  }
};

#endif // VM_PERSISTENCE_SLIB_MEM_ALLOC_H_

