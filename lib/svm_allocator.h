/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks. Threading Building Blocks is free software;
    you can redistribute it and/or modify it under the terms of the GNU General Public License
    version 2  as  published  by  the  Free Software Foundation.  Threading Building Blocks is
    distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See  the GNU General Public License for more details.   You should have received a copy of
    the  GNU General Public License along with Threading Building Blocks; if not, write to the
    Free Software Foundation, Inc.,  51 Franklin St,  Fifth Floor,  Boston,  MA 02110-1301 USA

    As a special exception,  you may use this file  as part of a free software library without
    restriction.  Specifically,  if other files instantiate templates  or use macros or inline
    functions from this file, or you compile this file and link it with other files to produce
    an executable,  this file does not by itself cause the resulting executable to be covered
    by the GNU General Public License. This exception does not however invalidate any other
    reasons why the executable file might be covered by the GNU General Public License.
*/

#ifndef VM_PERSISTENCE_SVM_ALLOCATOR_H_
#define VM_PERSISTENCE_SVM_ALLOCATOR_H_

#include "sitevm/sitevm_malloc.h"

//! Meets "allocator" requirements of ISO C++ Standard, Section 20.1.5
/** The class selects the best memory allocation mechanism available 
    from scalable_malloc and standard malloc.
    The members are ordered the same way they are in section 20.4.1
    of the ISO C++ standard.
    @ingroup memory_allocation */
template <typename T>
class SVMAllocator {
 public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  template<typename U> struct rebind {
    typedef SVMAllocator<U> other;
  };

  SVMAllocator() throw() { }
  SVMAllocator(const SVMAllocator &other) throw() { }
  template<typename U> SVMAllocator(const SVMAllocator<U> &other) throw() { }

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }
  
  //! Allocate space for n objects.
  pointer allocate(size_type n, const void *hint = 0) {
    return (pointer)sitevm_malloc::smalloc(n * sizeof(T));
  }

  //! Free previously allocated block of memory.
  void deallocate(pointer p, size_t n) {
    sitevm_malloc::sfree(p);
  }

  //! Largest value for which method allocate might succeed.
  size_type max_size() const throw() {
    size_type max = static_cast<size_type>(-1) / sizeof (value_type);
    return (max > 0 ? max : 1);
  }
  
  //! Copy-construct value at location pointed to by p.
#if __TBB_ALLOCATOR_CONSTRUCT_VARIADIC
  template<typename U, typename... Args>
  void construct(U *p, Args&&... args) {
    ::new((void *)p) U(std::forward<Args>(args)...);
  }
#else // __TBB_ALLOCATOR_CONSTRUCT_VARIADIC
#if __TBB_CPP11_RVALUE_REF_PRESENT
  void construct(pointer p, value_type&& value) {
    ::new((void*)(p)) value_type(std::move(value));
  }
#endif
  void construct(pointer p, const value_type &value) {
    ::new((void*)(p)) value_type(value);
  }
#endif // __TBB_ALLOCATOR_CONSTRUCT_VARIADIC

  //! Destroy value at location pointed to by p.
  void destroy(pointer p) { p->~value_type(); }
};

//! Analogous to std::allocator<void>, as defined in ISO C++ Standard, Section 20.4.1
/** @ingroup memory_allocation */
template<> 
class SVMAllocator<void> {
 public:
  typedef void* pointer;
  typedef const void* const_pointer;
  typedef void value_type;
  template<typename U> struct rebind {
    typedef SVMAllocator<U> other;
  };
};

template<typename T, typename U>
inline bool operator==(const SVMAllocator<T> &a, const SVMAllocator<U> &b) {
  return true;
}

template<typename T, typename U>
inline bool operator!=(const SVMAllocator<T> &a, const SVMAllocator<U> &b) {
  return false;
}

#endif // VM_PERSISTENCE_SVM_ALLOCATOR_H_

