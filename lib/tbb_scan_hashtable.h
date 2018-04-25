//
// tbb_scan_hashtable.h
//
// Created by Jinglei Ren on 12/28/14.
// Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_LIB_TBB_SCAN_HASHTABLE_H_
#define YCSB_C_LIB_TBB_SCAN_HASHTABLE_H_

#include "lib/string_hashtable.h"

#include <vector>
#include "tbb/concurrent_unordered_map.h"
#include "lib/string.h"

namespace vmp {

template<class V>
class TbbScanHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;
  TbbScanHashtable() { table_.max_load_factor(2.0); }

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key = NULL, std::size_t n = -1) const;
  std::size_t Size() const { return table_.size(); }

 private:
  struct Hash {
    uint64_t operator()(const String &hstr) const { return hstr.hash(); }
  };

  typedef tbb::concurrent_unordered_map<String, V, Hash> Hashtable;

  Hashtable table_;
};

template<class V>
V TbbScanHashtable<V>::Get(const char *key) const {
  typename Hashtable::const_iterator it = table_.find(String::Wrap(key));
  if (it == table_.end()) return NULL;
  return it->second;
}

template<class V>
bool TbbScanHashtable<V>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey = String::Copy<MemAlloc>(key);
  return table_.insert(std::make_pair(skey, value)).second;
}

template<class V>
V TbbScanHashtable<V>::Update(const char *key, V value) {
  V old(NULL);
  typename Hashtable::iterator it = table_.find(String::Wrap(key));
  if (it != table_.end()) {
    old = it->second;
    it->second = value;
  }
  return old;
}

template<class V>
V TbbScanHashtable<V>::Remove(const char *key) {
  assert(false); // Unsafe operation!
  V old(NULL);
  typename Hashtable::iterator it = table_.find(String::Wrap(key));
  if (it != table_.end()) {
    String::Free<MemAlloc>(it->first);
    old = it->second;
    table_.unsafe_erase(it);
  }
  return old;
}

template<class V>
std::vector<typename TbbScanHashtable<V>::KVPair> TbbScanHashtable<V>::Entries(
    const char *key, std::size_t n) const {
  std::vector<KVPair> pairs;
  typename Hashtable::const_iterator pos;
  pos = key ? table_.equal_range(String::Wrap(key)).first : table_.begin();
  for (std::size_t i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // YCSB_C_LIB_TBB_SCAN_HASHTABLE_H_

