//
//  slib_db.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_SLIB_DB_H_
#define YCSB_C_SLIB_DB_H_

#include "db/hashtable_db.h"

#include <string>
#include "sitevm/sitevm_malloc.h"
#include "lib/slib_hashtable.h"

namespace ycsbc {

class SLibDB : public HashtableDB {
 public:
  SLibDB();
  void Init();
  void Close();
  ~SLibDB();

  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields,
           std::vector<KVPair> &result);
  int Scan(const std::string &table, const std::string &key,
           int len, const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result);
  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);
  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);
  int Delete(const std::string &table, const std::string &key);
 protected:
  HashtableDB::FieldHashtable *NewFieldHashtable();
  void DeleteFieldHashtable(HashtableDB::FieldHashtable *table);
  const char *CopyString(const std::string &str);
  void DeleteString(const char *str);
 private:
  sitevm_seg_t* svm_;
};

} // ycsbc

#endif // YCSB_C_SLIB_DB_H_
