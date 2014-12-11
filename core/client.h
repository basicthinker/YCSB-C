//
//  client.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/10/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_CLIENT_H_
#define YCSB_C_CLIENT_H_

#include <string>

namespace ycsbc {

class Client {
 public:
  static const std::string OPERATION_COUNT_PROPERTY;
  static const std::string RECORD_COUNT_PROPERTY;
};

} // ycsbc

#endif // YCSB_C_CLIENT_H_
