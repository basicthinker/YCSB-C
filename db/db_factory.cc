//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include "db/basic_db.h"
#include "db/lock_stl_db.h"
#include "db/tbb_rand_db.h"
#include "db/tbb_scan_db.h"

using ycsbc::DB;
using ycsbc::DBFactory;

DB* DBFactory::CreateDB(const std::string name) {
  if (name == "basic") {
    return new BasicDB;
  } else if (name == "lock_stl") {
    return new LockStlDB;
  } else if (name == "tbb_rand") {
    return new TbbRandDB;
  } else if (name == "tbb_scan") {
    return new TbbScanDB;
  } else return NULL;
}

