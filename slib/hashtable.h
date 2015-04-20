//
//  hashtable.h
//  vm_persistence
//
//  Created by Jinglei Ren on 12/24/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_SLIB_HASHTABLE_H_
#define VM_PERSISTENCE_SLIB_HASHTABLE_H_

#include <cstdint>
#include "slib/pair.h"
#include "slib/list.h"
#include "slib/vector.h"
#include "slib/mem_alloc.h"
#include "slib/assert.h"

namespace slib {

template<typename T>
struct HashEqual {
  __attribute__((transaction_safe))
  virtual uint64_t hash(const T &key) const {
    std::hash<T> hasher;
    return hasher(key);
  }
  
  __attribute__((transaction_safe))
  virtual bool equal(const T &x, const T &y) const {
    std::equal_to<T> equal;
    return equal(x, y);
  }
  
  __attribute__((transaction_safe))
  virtual ~HashEqual() { }
};

struct hlist_bucket {
  std::size_t size;
  struct hlist_head head;
};

template <typename K, typename V>
struct hlist_pair {
  K key() const { return pair.first; }
  void set_key(const K &key) { pair.first = key; }
  V value() const { return pair.second; }
  void set_value(const V &value) { pair.second = value; }

  hlist_node node;

  hlist_pair(const K &k, const V &v) : pair(k, v) {
    INIT_HLIST_NODE(&node);
  }

 private:
  slib::pair<K, V> pair;
};

template <typename K, typename V, class HashEqual, class Alloc>
class hashtable {
 public:
  hashtable(std::size_t num_buckets = 11, std::size_t local_load_factor = 4);
  size_t local_load_factor() const { return local_load_factor_; }
  void set_local_load_factor(std::size_t f) { local_load_factor_ = f; }
  std::size_t bucket_count() const { return bucket_count_; }
  
  bool find(const K &key, V &value) const __attribute__((transaction_safe));

  bool update(const K &key, V &value) __attribute__((transaction_safe));

  bool insert(const K &key, const V &value) __attribute__((transaction_safe));

  __attribute__((transaction_safe))
  bool erase(const K &key, slib::pair<K, V> &erased);

  slib::vector<slib::pair<K, V>> entries(const K *key = NULL,
          std::size_t n = -1) const  __attribute__((transaction_safe));

  std::size_t clear() __attribute__((transaction_safe));

  void rehash(std::size_t buckets) __attribute__((transaction_safe));
  
  std::size_t size() const;
  std::size_t bucket_size(size_t i) const { return buckets_[i].size; }
  float load_factor() const;
  
  ~hashtable();

 private:
  hlist_bucket *get_bucket(const K &key) const {
    return buckets_ + hash_equal_.hash(key) % bucket_count_;
  }

  std::size_t local_load_factor_;
  hlist_bucket *buckets_;
  std::size_t bucket_count_;
  HashEqual hash_equal_;
};

// Accessory functions

template <class Alloc> __attribute__((transaction_safe))
inline hlist_bucket *new_buckets(std::size_t n) {
  hlist_bucket *bkts = (hlist_bucket *)Alloc::Malloc(n * sizeof(hlist_bucket));
  for (std::size_t i = 0; i < n; ++i) {
    bkts[i].size = 0;
    INIT_HLIST_HEAD(&bkts[i].head);
  }
  return bkts;
}

inline std::size_t size_sum(const hlist_bucket *bkts, std::size_t n) {
  std::size_t count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    count += bkts[i].size;
  }
  return count;
}

template <typename K, typename V, class HashEqual, class Alloc>
__attribute__((transaction_safe))
hlist_pair<K, V> *find_in(const hlist_bucket *bkt, const K &key,
    const HashEqual &hash_equal = HashEqual()) {
  hlist_node *node;
  hlist_for_each(node, &bkt->head) {
    hlist_pair<K, V> *kv_pair = container_of(node, &hlist_pair<K, V>::node);
    if (hash_equal.equal(kv_pair->key(), key)) {
      return kv_pair;
    }
  }
  return NULL;   
}

template <typename K, typename V>
void insert_to(hlist_bucket *bkt, hlist_pair<K, V> *kv_pair) {
  hlist_add_head(&kv_pair->node, &bkt->head);
  ++bkt->size;
}

template <typename K, typename V, class Alloc>
__attribute__((transaction_safe))
void erase_from(hlist_bucket *bkt, hlist_pair<K, V> *kv_pair) {
  hlist_del(&kv_pair->node);
  Alloc::Delete(kv_pair);
  --bkt->size;
  Assert((bool)bkt->size != hlist_empty(&bkt->head), "erase_from inconsistent");
}

template <typename K, typename V, class Alloc>
std::size_t clear_all(hlist_bucket *bkts, std::size_t n) {
  std::size_t num = 0;
  for (std::size_t i = 0; i < n; ++i) {
    hlist_node *pos, *next;
    hlist_for_each_safe(pos, next, &bkts[i].head) {
      hlist_pair<K, V> *kv_pair = container_of(pos, &hlist_pair<K, V>::node);
      erase_from<K, V, Alloc>(bkts + i, kv_pair);
      ++num;
    }
    Assert(!bkts[i].size && hlist_empty(&bkts[i].head), "clear_all leaks");
  }
  return num;
}

// Implementation of hashtable

template <typename K, typename V, class HashEqual, class Alloc>
hashtable<K, V, HashEqual, Alloc>::hashtable(std::size_t n, std::size_t f) {
  bucket_count_ = n;
  buckets_ = new_buckets<Alloc>(bucket_count_);
  local_load_factor_ = f;
}

template <typename K, typename V, class HashEqual, class Alloc>
bool hashtable<K, V, HashEqual, Alloc>::find(const K &key, V &value) const {
  hlist_bucket *bkt = get_bucket(key);
  hlist_pair<K, V> *kv_pair = find_in<K, V, HashEqual, Alloc>(bkt, key);
  if (!kv_pair) return false;
  value = kv_pair->value();
  return true;
}

template <typename K, typename V, class HashEqual, class Alloc>
bool hashtable<K, V, HashEqual, Alloc>::update(const K &key, V &value) {
  hlist_bucket *bkt = get_bucket(key);
  hlist_pair<K, V> *kv_pair = find_in<K, V, HashEqual, Alloc>(bkt, key);
  if (!kv_pair) return false;
  V old = kv_pair->value();
  kv_pair->set_value(value);
  value = old;
  return true;
}

template <typename K, typename V, class HashEqual, class Alloc>
bool hashtable<K, V, HashEqual, Alloc>::insert(const K &key, const V &value) {
  hlist_bucket *bkt = get_bucket(key);
  hlist_pair<K, V> *kv_pair = find_in<K, V, HashEqual, Alloc>(bkt, key); 
  if (kv_pair) return false;
  kv_pair = Alloc::template New<hlist_pair<K, V>>(key, value);
  insert_to(bkt, kv_pair);

  if (bkt->size >= local_load_factor_) {
    rehash((bucket_count_ << 1) + 1);
  }
  return true;
}

template <typename K, typename V, class HashEqual, class Alloc>
bool hashtable<K, V, HashEqual, Alloc>::erase(
    const K &key, slib::pair<K, V> &erased) {
  hlist_bucket *bkt = get_bucket(key);
  hlist_pair<K, V> *kv_pair = find_in<K, V, HashEqual, Alloc>(bkt, key);
  if (!kv_pair) return false;
  erased.first = kv_pair->key();
  erased.second = kv_pair->value();
  erase_from<K, V, Alloc>(bkt, kv_pair);
  return true;
}

template <typename K, typename V, class HashEqual, class Alloc>
slib::vector<slib::pair<K, V>> hashtable<K, V, HashEqual, Alloc>::entries(
    const K *key, std::size_t num) const {
  if (num > bucket_count_) num = bucket_count_;
  slib::vector<slib::pair<K, V>> pairs(num);
  hlist_bucket *bkt = key ? get_bucket(*key) : buckets_;
  hlist_pair<K, V> *pos = key ? find_in<K, V, HashEqual, Alloc>(bkt, *key) :
      container_of(bkt->head.first, &hlist_pair<K, V>::node);
  if (!pos) return pairs;

  hlist_bucket *bkt_end = buckets_ + bucket_count_;
  hlist_node *node = &pos->node;
  for (; bkt < bkt_end; ++bkt) {
    if (!node) node = bkt->head.first;
    for (; node; node = node->next) {
      hlist_pair<K, V> *kv_pair = container_of(node, &hlist_pair<K, V>::node);
      pairs.push_back(slib::make_pair(kv_pair->key(), kv_pair->value()));
      if (pairs.size() == num) return pairs;
    }
  }
  return pairs;
}

template <typename K, typename V, class HashEqual, class Alloc>
std::size_t hashtable<K, V, HashEqual, Alloc>::clear() {
  return clear_all<K, V, Alloc>(buckets_, bucket_count_);
}

template <typename K, typename V, class HashEqual, class Alloc>
hashtable<K, V, HashEqual, Alloc>::~hashtable() {
  clear_all<K, V, Alloc>(buckets_, bucket_count_);
  Alloc::Free(buckets_, sizeof(hlist_bucket) * bucket_count_);
}

template <typename K, typename V, class HashEqual, class Alloc>
void hashtable<K, V, HashEqual, Alloc>::rehash(std::size_t n) {
  //Assert(false, "REHASH\n");
  hlist_bucket *bkts = new_buckets<Alloc>(n);
  std::size_t num = 0;
  for (std::size_t i = 0; i < bucket_count_; ++i) {
    hlist_node *pos, *next;
    hlist_for_each_safe(pos, next, &buckets_[i].head) {
      hlist_pair<K, V> *kv_pair = container_of(pos, &hlist_pair<K, V>::node);
      std::size_t j = hash_equal_.hash(kv_pair->key()) % n;
      insert_to(bkts + j, kv_pair);
      ++num;
    }
  }
  Alloc::Free(buckets_, sizeof(hlist_bucket) * bucket_count_);
  buckets_ = bkts;
  bucket_count_ = n;
}

template <typename K, typename V, class HashEqual, class Alloc>
std::size_t hashtable<K, V, HashEqual, Alloc>::size() const {
  return size_sum(buckets_, bucket_count_);
}

template <typename K, typename V, class HashEqual, class Alloc>
float hashtable<K, V, HashEqual, Alloc>::load_factor() const {
  return size() / (float)bucket_count_;
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_HASHTABLE_H_

