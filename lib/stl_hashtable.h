//
//  stl_hashtable.h
//
//  Created by Jinglei Ren on 12/22/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_LIB_STL_HASHTABLE_H_
#define YCSB_C_LIB_STL_HASHTABLE_H_

#include "lib/string_hashtable.h"

#include <unordered_map>
#include <vector>
#include "lib/string.h"

namespace vmp {

template <class V, class MA = MemAlloc,
    class PA = std::allocator<std::pair<String, V>>>
class StlHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  StlHashtable(std::size_t num_buckets = 11, float max_load_factor = 2.0);

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

  typedef std::unordered_map<String, V, Hash, Equal, PA> Hashtable;
  Hashtable table_;
};

template<class V, class MA, class PA>
StlHashtable<V, MA, PA>::StlHashtable(std::size_t n, float f) : table_(n) {
  table_.max_load_factor(f);
}

template<class V, class MA, class PA>
V StlHashtable<V, MA, PA>::Get(const char *key) const {
  typename Hashtable::const_iterator pos = table_.find(String::Wrap(key));
  if (pos == table_.end()) return NULL;
  else return pos->second;
}

template<class V, class MA, class PA>
bool StlHashtable<V, MA, PA>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey = String::Copy<MA>(key);
  return table_.insert(std::make_pair(skey, value)).second;
}

template<class V, class MA, class PA>
V StlHashtable<V, MA, PA>::Update(const char *key, V value) {
  typename Hashtable::iterator pos = table_.find(String::Wrap(key));
  if (pos == table_.end()) return NULL;
  V old = pos->second;
  pos->second = value;
  return old;
}

template<class V, class MA, class PA>
V StlHashtable<V, MA, PA>::Remove(const char *key) {
  typename Hashtable::const_iterator pos = table_.find(String::Wrap(key));
  if (pos == table_.end()) return NULL;
  String::Free<MA>(pos->first);
  V old = pos->second;
  table_.erase(pos);
  return old;
}

template<class V, class MA, class PA>
std::vector<typename StlHashtable<V, MA, PA>::KVPair>
StlHashtable<V, MA, PA>::Entries(const char *key, std::size_t n) const {
  std::vector<KVPair> pairs;
  typename Hashtable::const_iterator pos;
  if (!key) {
    pos = table_.cbegin();
  } else {
    pos = table_.find(String::Wrap(key));
  }
  for (std::size_t i = 0; pos != table_.end() && i < n; ++pos, ++i) {
    pairs.push_back(std::make_pair(pos->first.value(), pos->second));
  }
  return pairs;
}

} // vmp

#endif // YCSB_C_LIB_STL_HASHTABLE_H_

