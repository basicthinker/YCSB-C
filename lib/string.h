//
//  hash_string.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_SLIB_STRING_H_
#define YCSB_C_SLIB_STRING_H_

#include <cstring>
#include <cassert>
#include <cstdint>

#include "slib/mem_alloc.h"
#include "slib/string.h"

namespace vmp {

__attribute__((transaction_safe))
static int strncmp(const char *s1, const char *s2, size_t n) {
  if (!n) return 0;

  while (--n && *s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

class String {
 public:
  String() : hash_(0), value_(NULL), len_(0) { }
  uint64_t hash() const { return hash_; }
  const char *value() const { return value_; }
  size_t length() const { return len_; }
  void set_value(const char *v);

  template <class Alloc> __attribute__((transaction_safe))
  static String Copy(const char *v);

  static String Wrap(const char *v) __attribute__((transaction_safe));

  template <class Alloc> __attribute__((transaction_safe))
  static void Free(const String& str);

  bool operator==(const String &other) const;

 private:
  uint64_t SDBMHash(const char *str);

  uint64_t hash_;
  const char *value_;
  size_t len_;
};

inline void String::set_value(const char *v) {
  value_ = v;
  len_ = slib::strlen(value_);
  hash_ = SDBMHash(value_);
}

inline uint64_t String::SDBMHash(const char *cstr) {
  uint64_t hash = 0;
  uint64_t c;
  while ((c = *cstr++) != '\0') {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}

template <class Alloc>
inline String String::Copy(const char *cstr) {
  slib::Assert(cstr, "String::Copy null string");
  String hstr;
  const size_t len = slib::strlen(cstr);
  char *str = (char *)Alloc::Malloc(len + 1);
  memcpy((void*)str, (void*)cstr, len + 1);
  hstr.set_value(str);
  slib::Assert(hstr.length() == len, "String::Copy inconsistent length");
  return hstr;
}

inline String String::Wrap(const char *cstr) {
  slib::Assert(cstr, "String::Wrap null string");
  String hstr;
  hstr.set_value(cstr);
  return hstr;
}

template <class Alloc>
inline void String::Free(const String& hstr) {
  Alloc::Free(hstr.value(), hstr.length() + 1);
}

inline bool String::operator==(const String &other) const {
  if (hash_ != other.hash() || len_ != other.length()) return false;
  return strncmp(value_, other.value(), len_) == 0;
}

} // slib

#endif // YCSB_C_SLIB_STRING_H_

