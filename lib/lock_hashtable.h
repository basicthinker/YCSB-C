//
//  lock_hashtable.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_LOCK_HASHTABLE_H_
#define VM_PERSISTENCE_LOCK_HASHTABLE_H_

#include "lib/stl_hashtable.h"

#include <vector>
#include <mutex>

namespace vmp {

template<class V>
class LockHashtable : public STLHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key, size_t n) const;
  std::size_t Size() const;

 private:
  mutable std::mutex mutex_;
};

template<class V>
inline V LockHashtable<V>::Get(const char *key) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Get(key);
}

template<class V>
inline bool LockHashtable<V>::Insert(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Insert(key, value);
}

template<class V>
inline V LockHashtable<V>::Update(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Update(key, value);
}

template<class V>
inline V LockHashtable<V>::Remove(const char *key) {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Remove(key);
}

template<class V>
inline std::size_t LockHashtable<V>::Size() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Size();
}

template<class V>
inline std::vector<typename LockHashtable<V>::KVPair> LockHashtable<V>::Entries(
    const char *key, size_t n) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return STLHashtable<V>::Entries(key, n);
}

} // vmp

#endif // VM_PERSISTENCE_LOCK_HASHTABLE_H_

