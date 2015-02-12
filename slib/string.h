//
//  string.h
//  vm-persistence
//
//  Created by Jinglei Ren on Jan 19 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_SLIB_STRING_H_
#define VM_PERSISTENCE_SLIB_STRING_H_

namespace slib {

__attribute__((transaction_safe))
static std::size_t strlen(const char *str) {
  register const char *s;
  for (s = str; *s; ++s);
  return (s - str);
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_PAIR_H_

