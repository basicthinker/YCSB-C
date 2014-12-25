//
//  hash_string.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_HASH_STRING_H_
#define VM_PERSISTENCE_HASH_STRING_H_

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdint>

#include "lib/mem_allocator.h"

namespace vmp {

class String {
 public:
  String() : hash_(0), value_(NULL), len_(0) { }
  explicit String(const char *str);
  uint64_t hash() const { return hash_; }
  const char *value() const { return value_; }
  size_t length() const { return len_; }
  void set_value(const char *v);

  static String Wrap(const char *v);

 private:
  uint64_t SDBMHash(const char *str);

  uint64_t hash_;
  const char *value_;
  size_t len_;
};

inline String::String(const char *cstr) {
  assert(cstr);
  const size_t len = strlen(cstr); 
  char *hstr = (char *)MALLOC(len + 1);
  memcpy(hstr, cstr, len);
  hstr[len] = '\0';
  set_value(hstr);
  assert(length() == len);
}

inline void String::set_value(const char *v) {
  value_ = v;
  len_ = strlen(value_);
  hash_ = SDBMHash(value_);
}

inline uint64_t String::SDBMHash(const char *str) {
  uint64_t hash = 0;
  uint64_t c;
  while ((c = *str++) != '\0') {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}

inline String String::Wrap(const char *str) {
  String hstr;
  hstr.set_value(str);
  return hstr;
}

} // vmp

#endif // VM_PERSISTENCE_HASH_STRING_H_

