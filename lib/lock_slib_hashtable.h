//
//  lock_slib_hashtable.h
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>
//

#ifndef YCSB_C_LIB_LOCK_SLIB_HASHTABLE_H_
#define YCSB_C_LIB_LOCK_SLIB_HASHTABLE_H_

#include "lib/slib_hashtable.h"

#include <vector>
#include <mutex>
#include "slib/mem_alloc.h"

namespace vmp {

template<class V>
class LockSlibHashtable : public SlibHashtable<V, slib::MemAlloc> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key = NULL, size_t n = -1) const;
  std::size_t Size() const;

 private:
  mutable std::mutex mutex_;
};

template<class V>
inline V LockSlibHashtable<V>::Get(const char *key) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Get(key);
}

template<class V>
inline bool LockSlibHashtable<V>::Insert(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Insert(key, value);
}

template<class V>
inline V LockSlibHashtable<V>::Update(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Update(key, value);
}

template<class V>
inline V LockSlibHashtable<V>::Remove(const char *key) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Remove(key);
}

template<class V>
inline std::size_t LockSlibHashtable<V>::Size() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Size();
}

template<class V>
inline std::vector<typename LockSlibHashtable<V>::KVPair>
LockSlibHashtable<V>::Entries(const char *key, size_t n) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SlibHashtable<V, slib::MemAlloc>::Entries(key, n);
}

} // vmp

#endif // YCSB_C_LIB_LOCK_SLIB_HASHTABLE_H_

