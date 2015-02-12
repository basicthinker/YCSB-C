//
//  itm_slib_hashtable.h
//
//  Created by Jinglei Ren on Jan 14 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef YCSB_C_LIB_ITM_SLIB_HASHTABLE_H_
#define YCSB_C_LIB_ITM_SLIB_HASHTABLE_H_

#include "lib/slib_hashtable.h"

#include <vector>
#include <mutex>
#include "slib/mem_alloc.h"

namespace vmp {

template<class V, class Alloc>
class ItmSlibHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns nullptr if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key = nullptr,
                              std::size_t n = -1) const;
  std::size_t Size() const;

 private:
  struct HashEqual {
    __attribute__((transaction_safe))
    uint64_t hash(const String &hstr) const { return hstr.hash(); }
    __attribute__((transaction_safe))
    bool equal(const String &a, const String &b) const { return a == b; }
  };

  typedef typename
      slib::hashtable<String, V, HashEqual, Alloc> Hashtable;
  Hashtable table_;
};

template<class V, class Alloc>
inline V ItmSlibHashtable<V, Alloc>::Get(const char *key) const {
  String skey = String::Wrap(key);
  V value;
  __transaction_atomic {
    if (!table_.find(skey, value)) value = nullptr;
    return value;
  }
}

template<class V, class Alloc>
inline bool ItmSlibHashtable<V, Alloc>::Insert(const char *key, V value) {
  if (!key) return false;
  __transaction_atomic {
    String skey = String::Copy<Alloc>(key);
    return table_.insert(skey, value);
  }
}

template<class V, class Alloc>
inline V ItmSlibHashtable<V, Alloc>::Update(const char *key, V value) {
  String skey = String::Wrap(key);
  __transaction_atomic {
    if (!table_.update(skey, value)) return nullptr;
    return value;
  }
}

template<class V, class Alloc>
inline V ItmSlibHashtable<V, Alloc>::Remove(const char *key) {
  String skey = String::Wrap(key);
  slib::pair<String, V> removed;
  __transaction_atomic {
    if (!table_.erase(skey, removed)) return nullptr;
    String::Free<Alloc>(removed.first);
    return removed.second;
  }
}

template<class V, class Alloc>
inline std::size_t ItmSlibHashtable<V, Alloc>::Size() const {
  __transaction_atomic {
    return table_.size();
  }
}

template<class V, class Alloc>
std::vector<typename ItmSlibHashtable<V, Alloc>::KVPair>
ItmSlibHashtable<V, Alloc>::Entries(const char *key, size_t n) const {
  String skey;
  slib::vector<slib::pair<String, V>> entries;
  __transaction_atomic {
    entries = table_.entries((key ? skey = String::Wrap(key), &skey : nullptr), n);
  }
  std::vector<KVPair> results;
  for (auto entry : entries) {
    results.push_back(std::make_pair(entry.first.value(), entry.second));
  }
  return results;
}

} // vmp

#endif // YCSB_C_LIB_ITM_SLIB_HASHTABLE_H_

