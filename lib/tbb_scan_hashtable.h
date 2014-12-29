//
// tbb_scan_hashtable.h
//
// Created by Jinglei Ren on 12/28/14.
// Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_TBB_SCAN_HASHTABLE_H_
#define VM_PERSISTENCE_TBB_SCAN_HASHTABLE_H_

#include "lib/string_hashtable.h"

#include <vector>
#include "tbb/concurrent_unordered_map.h"
#include "tbb/queuing_rw_mutex.h"
#include "lib/string.h"

namespace vmp {

template<class V>
class TBBScanHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

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
  mutable tbb::queuing_rw_mutex mutex_;
};

template<class V>
V TBBScanHashtable<V>::Get(const char *key) const {
  tbb::queuing_rw_mutex::scoped_lock lock(mutex_, false);
  typename Hashtable::const_iterator it = table_.find(String::Wrap(key));
  if (it == table_.end()) return NULL;
  return it->second;
}

template<class V>
bool TBBScanHashtable<V>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey(key);
  tbb::queuing_rw_mutex::scoped_lock lock(mutex_, false);
  return table_.insert(std::make_pair(skey, value)).second;
}

template<class V>
V TBBScanHashtable<V>::Update(const char *key, V value) {
  V old(NULL);
  tbb::queuing_rw_mutex::scoped_lock lock(mutex_, false);
  typename Hashtable::iterator it = table_.find(String::Wrap(key));
  if (it != table_.end()) {
    old = it->second;
    it->second = value;
  }
  return old;
}

template<class V>
V TBBScanHashtable<V>::Remove(const char *key) {
  V old(NULL);
  tbb::queuing_rw_mutex::scoped_lock lock(mutex_);
  typename Hashtable::iterator it = table_.find(String::Wrap(key));
  if (it != table_.end()) {
    FREE(it->first.value());
    old = it->second;
    table_.unsafe_erase(it);
  }
  return old;
}

template<class V>
std::vector<typename TBBScanHashtable<V>::KVPair> TBBScanHashtable<V>::Entries(
    const char *key, size_t n) const {
  std::vector<KVPair> pairs;
  typename Hashtable::const_iterator pos;
  tbb::queuing_rw_mutex::scoped_lock lock(mutex_, false);
  pos = key ? table_.equal_range(String::Wrap(key)).first : table_.begin();
  for (int i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // VM_PERSISTENCE_TBB_SCAN_HASHTABLE_H_

