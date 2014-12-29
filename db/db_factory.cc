//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include "db/lock_db.h"
#include "db/tbb_rand_db.h"
#include "db/tbb_scan_db.h"
#include "db/svm_db.h"
#include "db/slib_db.h"
#include "db/lock_slib_db.h"

using ycsbc::DB;
using ycsbc::DBFactory;

DB* DBFactory::CreateDB(const std::string name) {
  if (name == "basic") {
    return new BasicDB;
  } else if (name == "lock_db") {
    return new LockDB;
  } else if (name == "tbb_rand_db") {
    return new TBBRandDB;
  } else if (name == "tbb_scan_db") {
    return new TBBScanDB;
  } else if (name == "svm_db") {
    return new SVMDB;
  } else if (name == "slib_db") {
    return new SLibDB;
  } else if (name == "lock_slib_db") {
    return new LockSLibDB;
  } else return NULL;
}

