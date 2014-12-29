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

  STLHashtable(std::size_t num_buckets = 11, float max_load_factor = 2.0);

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key = NULL,
                              std::size_t n = -1) const;
  std::size_t Size() const { return table_.size(); }

 private:
  struct Hash {
    uint64_t operator()(const String &hstr) const { return hstr.hash(); }
  };

  struct Equal {
    bool operator()(const String &a, const String &b) const { return a == b; }
  };

  typedef std::unordered_map<String, V, Hash, Equal, A> Hashtable;
  Hashtable table_;
};

template<class V, class A>
STLHashtable<V, A>::STLHashtable(std::size_t n, float f) : table_(n) {
  table_.max_load_factor(f);
}

template<class V, class A>
V STLHashtable<V, A>::Get(const char *key) const {
  typename Hashtable::const_iterator pos = table_.find(String::Wrap(key));
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
  typename Hashtable::iterator pos = table_.find(String::Wrap(key));
  if (pos == table_.end()) return NULL;
  V old = pos->second;
  pos->second = value;
  return old;
}

template<class V, class A>
V STLHashtable<V, A>::Remove(const char *key) {
  typename Hashtable::const_iterator pos = table_.find(String::Wrap(key));
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
  typename Hashtable::const_iterator pos;
  if (!key) {
    pos = table_.cbegin();
  } else {
    pos = table_.find(String::Wrap(key));
  }
  for (int i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // VM_PERSISTENCE_STL_HASHTABLE_H_
