//
//  assert.h
//  vm-persistence
//
//  Created by Jinglei Ren on Jan 19 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_SLIB_ASSERT_H_
#define VM_PERSISTENCE_SLIB_ASSERT_H_

#include <iostream>

namespace slib {

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

__attribute__((transaction_pure))
inline void Assert(bool expression, const char *hint = "(no info)") {
  if (likely(expression)) return;
  std::cerr << "Assertion failed: " << hint << std::endl;
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_ASSERT_H_

