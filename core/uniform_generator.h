//
//  uniform_generator.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/6/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_UNIFORM_GENERATOR_H_
#define YCSB_C_UNIFORM_GENERATOR_H_

#include <random>
#include "generator.h"

namespace ycsbc {

class UniformGenerator : public IntGenerator {
 public:
  // Both min and max are inclusive
  UniformGenerator(uint64_t min, uint64_t max) : dist_(min, max) { }
  
  uint64_t NextInt() { return last_int_ = dist_(generator_); }
  uint64_t LastInt() { return last_int_; }
  
 private:
  uint64_t last_int_;
  std::mt19937_64 generator_;
  std::uniform_int_distribution<uint64_t> dist_;
};

} // ycsbc

#endif // YCSB_C_UNIFORM_GENERATOR_H_
