//
//  generator.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/6/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_GENERATOR_H_
#define YCSB_C_GENERATOR_H_

#include <cstdint>
#include <string>

namespace ycsbc {

class IntGenerator {
 public:
  virtual uint64_t NextInt() = 0;
  virtual uint64_t LastInt() = 0;
};

class StringGenerator {
 public:
  virtual std::string NextString() = 0;
  virtual std::string LastString() = 0;
};

} // ycsbc

#endif // YCSB_C_GENERATOR_H_
