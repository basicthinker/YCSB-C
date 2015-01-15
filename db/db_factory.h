//
//  db.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/18/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_DB_FACTORY_H_
#define YCSB_C_DB_FACTORY_H_

#include "core/db.h"

namespace ycsbc {

class DBFactory {
 public:
  static DB* CreateDB(const std::string name);
};

} // ycsbc

#endif // YCSB_C_DB_FACTORY_H_

