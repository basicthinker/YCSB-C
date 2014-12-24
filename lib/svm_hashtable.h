//
//  svm_hashtable.h
//
//  Created by Jinglei Ren on 12/23/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_SVM_HASHTABLE_H_
#define VM_PERSISTENCE_SVM_HASHTABLE_H_

#include "lib/stl_hashtable.h"

#include "sitevm/sitevm.h"
#include "lib/string.h"
#include "lib/svm_allocator.h"

namespace vmp {

///
/// A simple wrapper for the STL-based hash table with SVM allocator
/// No transactions are embodied in this layer where the template value is
/// not managed, because aborting a transaction may cause loss of values.
///
template<class V>
class SVMHashtable
      : public STLHashtable<V, SVMAllocator<std::pair<String, V>>> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;
  typedef SVMAllocator<std::pair<String, V>> Allocator;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key, size_t n) const;
  size_t Size() const;
};

template<class V>
V SVMHashtable<V>::Get(const char *key) const {
  return STLHashtable<V, Allocator>::Get(key);
}

template<class V>
bool SVMHashtable<V>::Insert(const char *key, V value) {
  return STLHashtable<V, Allocator>::Insert(key, value);
}

template<class V>
V SVMHashtable<V>::Update(const char *key, V value) {
  return STLHashtable<V, Allocator>::Update(key, value);
}

template<class V>
V SVMHashtable<V>::Remove(const char *key) {
  return STLHashtable<V, Allocator>::Remove(key);
}

template<class V>
std::size_t SVMHashtable<V>::Size() const {
  return STLHashtable<V, Allocator>::Size();
}

template<class V>
std::vector<typename SVMHashtable<V>::KVPair> SVMHashtable<V>::Entries(
    const char *key, size_t n) const {
  return STLHashtable<V, Allocator>::Entries(key, n);
}

} // vmp

#endif // VM_PERSISTENCE_SVM_HASHTABLE_H_

