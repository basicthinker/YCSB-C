//
//  assert.h
//  vm-persistence
//
//  Created by Jinglei Ren on Jan 19 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_SLIB_ASSERT_H_
#define VM_PERSISTENCE_SLIB_ASSERT_H_

#include <cstdarg>
#include <cstdio>

namespace slib {

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

__attribute__((transaction_pure))
inline void Assert(bool expression, const char *fmt, ...) {
  if (likely(expression)) return;

  va_list list;
  va_start(list, fmt);
  vfprintf (stderr, fmt, list);
  va_end(list);
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_ASSERT_H_

