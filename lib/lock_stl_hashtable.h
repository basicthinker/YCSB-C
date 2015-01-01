//
//  lock_stl_hashtable.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>
//

#ifndef YCSB_C_LIB_LOCK_STL_HASHTABLE_H_
#define YCSB_C_LIB_LOCK_STL_HASHTABLE_H_

#include "lib/stl_hashtable.h"

#include <vector>
#include <mutex>

namespace vmp {

template<class V>
class LockStlHashtable : public StlHashtable<V> {
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
inline V LockStlHashtable<V>::Get(const char *key) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Get(key);
}

template<class V>
inline bool LockStlHashtable<V>::Insert(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Insert(key, value);
}

template<class V>
inline V LockStlHashtable<V>::Update(const char *key, V value) {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Update(key, value);
}

template<class V>
inline V LockStlHashtable<V>::Remove(const char *key) {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Remove(key);
}

template<class V>
inline std::size_t LockStlHashtable<V>::Size() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Size();
}

template<class V>
inline std::vector<typename LockStlHashtable<V>::KVPair>
LockStlHashtable<V>::Entries(const char *key, size_t n) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return StlHashtable<V>::Entries(key, n);
}

} // vmp

#endif // YCSB_C_LIB_LOCK_STL_HASHTABLE_H_

