//
//  db_factory.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include <cstdlib>
#include "db/lock_stl_db.h"
#include "db/lock_slib_db.h"
#include "db/tbb_rand_db.h"
#include "db/tbb_scan_db.h"
#include "db/itm_slib_db.h"
#include "db/aerospike_db.h"

using ycsbc::DB;
using ycsbc::DBFactory;

DB *DBFactory::CreateDB(const std::string name) {
  if (name == "basic") {
    return new BasicDB;
  } else if (name == "lock_stl") {
    return new LockStlDB;
  } else if (name == "lock_slib") {
    return new LockSlibDB;
  } else if (name == "tbb_rand") {
    return new TbbRandDB;
  } else if (name == "tbb_scan") {
    return new TbbScanDB;
  } else if (name == "itm_slib") {
    char *method = std::getenv("ITM_DEFAULT_METHOD");
    if (method && strncmp(method, "svm", 3) == 0) {
      return slib::SvmAlloc::New<ItmSlibDB<slib::SvmAlloc>>();
    } else {
      return new ItmSlibDB<slib::MemAlloc>;
    }
  } else if (name == "aerospike") {
    AerospikeDB *db = new AerospikeDB;
    db->Open("localhost", 3000);
    return db;
  } else return NULL;
}

void DBFactory::DestroyDB(DB *db) {
  char *method = std::getenv("ITM_DEFAULT_METHOD");
  if (method && strncmp(method, "svm", 3) == 0) {
    slib::SvmAlloc::Delete(db);
  } else {
    delete db;
  }
}

