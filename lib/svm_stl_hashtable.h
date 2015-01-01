//
//  svm_stl_hashtable.h
//
//  Created by Jinglei Ren on 12/23/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_LIB_SVM_HASHTABLE_H_
#define YCSB_C_LIB_SVM_HASHTABLE_H_

#include "lib/stl_hashtable.h"

#include "lib/string.h"
#include "lib/svm_allocator.h"

namespace vmp {

///
/// A simple wrapper for the STL-based hash table with SVM allocator
/// No transactions are embodied in this layer where the template value is
/// not managed, because aborting a transaction may cause loss of values.
///
template<class V>
class SvmHashtable
      : public StlHashtable<V, SvmAlloc, SvmAllocator<std::pair<String, V>>> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;
  typedef SvmAllocator<std::pair<String, V>> PairAlloc;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key, size_t n) const;
  size_t Size() const;
};

template<class V>
V SvmHashtable<V>::Get(const char *key) const {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Get(key);
}

template<class V>
bool SvmHashtable<V>::Insert(const char *key, V value) {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Insert(key, value);
}

template<class V>
V SvmHashtable<V>::Update(const char *key, V value) {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Update(key, value);
}

template<class V>
V SvmHashtable<V>::Remove(const char *key) {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Remove(key);
}

template<class V>
std::size_t SvmHashtable<V>::Size() const {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Size();
}

template<class V>
std::vector<typename SvmHashtable<V>::KVPair> SvmHashtable<V>::Entries(
    const char *key, size_t n) const {
  return StlHashtable<V, SvmAlloc, PairAlloc>::Entries(key, n);
}

} // vmp

#endif // YCSB_C_LIB_SVM_HASHTABLE_H_

