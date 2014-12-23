//
//  stl_hashtable.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_STL_HASHTABLE_H_
#define VM_PERSISTENCE_STL_HASHTABLE_H_

#include "lib/string_hashtable.h"

#include <unordered_map>
#include <vector>
#include "lib/string.h"

namespace vmp {

template <class V, class A = std::allocator<std::pair<String, V>>>
class STLHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key, std::size_t n) const;
  std::size_t Size() const { return table_.size(); }

 private:
  struct Hash {
    uint64_t operator()(const String &hstr) const { return hstr.hash(); }
  };

  struct Equal {
    bool operator()(const String &a, const String &b) const {
      if (a.hash() != b.hash()) return false;
      return strcmp(a.value(), b.value()) == 0;
    }
  };

  typedef std::unordered_map<String, V, Hash, Equal, A> Hashtable;
  Hashtable table_;
};

template<class V, class A>
V STLHashtable<V, A>::Get(const char *key) const {
  String skey;
  skey.set_value(key);
  typename Hashtable::const_iterator pos = table_.find(skey);
  if (pos == table_.end()) return NULL;
  else return pos->second;
}

template<class V, class A>
bool STLHashtable<V, A>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey(key);
  return table_.insert(std::make_pair(skey, value)).second;
}

template<class V, class A>
V STLHashtable<V, A>::Update(const char *key, V value) {
  String skey;
  skey.set_value(key);
  typename Hashtable::iterator pos = table_.find(skey);
  if (pos == table_.end()) return NULL;
  V old = pos->second;
  pos->second = value;
  return old;
}

template<class V, class A>
V STLHashtable<V, A>::Remove(const char *key) {
  String skey;
  skey.set_value(key);
  typename Hashtable::const_iterator pos = table_.find(skey);
  if (pos == table_.end()) return NULL;
  FREE(pos->first.value());
  V old = pos->second;
  table_.erase(pos);
  return old;
}

template<class V, class A>
std::vector<typename STLHashtable<V, A>::KVPair> STLHashtable<V, A>::Entries(
    const char *key, size_t n) const {
  std::vector<KVPair> pairs;
  String skey;
  skey.set_value(key);
  typename Hashtable::const_iterator pos = table_.find(skey);
  for (int i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // VM_PERSISTENCE_STL_HASHTABLE_H_
