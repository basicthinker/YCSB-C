//
//  pair.h
//  vm-persistence
//
//  Created by Jinglei Ren on Jan 18 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef VM_PERSISTENCE_SLIB_PAIR_H_
#define VM_PERSISTENCE_SLIB_PAIR_H_

namespace slib {

template <class T1, class T2>
struct pair {
  T1 first;
  T2 second;

  pair() { }
  pair(const T1 &f, const T2 &s) : first(f), second(s) { }
};

template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1 &f, const T2 &s) {
  return {f, s};
}

} // namespace slib

#endif // VM_PERSISTENCE_SLIB_PAIR_H_

