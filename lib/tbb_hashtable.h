//
// tbb_hashtable.h
//
// Created by Jinglei Ren on 12/22/14.
// Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_TBB_HASHTABLE_H_
#define VM_PERSISTENCE_TBB_HASHTABLE_H_

#include "lib/string_hashtable.h"

#include <vector>
#include "tbb/concurrent_hash_map.h"
#include "lib/string.h"

namespace vmp {

template<class V>
class TBBHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key, std::size_t n) const;
  std::size_t Size() const { return table_.size(); }

 private:
  struct HashCompare {
    inline uint64_t hash(const String &hstr) const {
      return hstr.hash();
    }

    inline bool equal(const String &a, const String &b) const {
      if (a.hash() != b.hash()) return false;
      return strcmp(a.value(), b.value()) == 0;
    }
  };

  typedef tbb::concurrent_hash_map<String, V, HashCompare> Hashtable;

  Hashtable table_;
};

template<class V>
V TBBHashtable<V>::Get(const char *key) const {
  String skey;
  skey.set_value(key);
  typename Hashtable::accessor result;
  if (!table_.find(result, skey)) return NULL;
  return result->second;
}

template<class V>
bool TBBHashtable<V>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey(key);
  return table_.insert(std::make_pair(skey, value));
}

template<class V>
V TBBHashtable<V>::Update(const char *key, V value) {
  String skey;
  skey.set_value(key);
  typename Hashtable::accessor result;
  if (!table_.find(result, skey)) return NULL;
  V old = result->second;
  result->second = value;
  return old;
}

template<class V>
V TBBHashtable<V>::Remove(const char *key) {
  String skey;
  skey.set_value(key);
  typename Hashtable::accessor result;
  if (!table_.find(result, skey)) return NULL;
  FREE(result->first.value());
  V old = result->second;
  table_.erase(result);
  return old;
}

template<class V>
std::vector<typename TBBHashtable<V>::KVPair> TBBHashtable<V>::Entries(
    const char *key, size_t n) const {
  std::vector<KVPair> pairs;
  String skey;
  skey.set_value(key);
  typename Hashtable::const_iterator pos = table_.equal_range(skey).first;
  for (int i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // VM_PERSISTENCE_TBB_HASHTABLE_H_

