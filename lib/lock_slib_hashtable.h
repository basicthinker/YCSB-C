//
//  lock_slib_hashtable.h
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_LOCK_SLIB_HASHTABLE_H_
#define VM_PERSISTENCE_LOCK_SLIB_HASHTABLE_H_

#include "lib/slib_hashtable.h"

#include <vector>
#include <mutex>

namespace vmp {

template<class V>
class LockSLibHashtable : public SLibHashtable<V> {
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
inline V LockSLibHashtable<V>::Get(const char *key) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Get(key);
}

template<class V>
inline bool LockSLibHashtable<V>::Insert(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Insert(key, value);
}

template<class V>
inline V LockSLibHashtable<V>::Update(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Update(key, value);
}

template<class V>
inline V LockSLibHashtable<V>::Remove(const char *key) {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Remove(key);
}

template<class V>
inline std::size_t LockSLibHashtable<V>::Size() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Size();
}

template<class V>
inline std::vector<typename LockSLibHashtable<V>::KVPair> LockSLibHashtable<V>::Entries(
    const char *key, size_t n) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return SLibHashtable<V>::Entries(key, n);
}

} // vmp

#endif // VM_PERSISTENCE_LOCK_SLIB_HASHTABLE_H_

