//
//  itm_slib_db.h
//  YCSB-C
//
//  Created by Jinglei Ren on Jan 14 2015.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_ITM_SLIB_DB_H_
#define YCSB_C_ITM_SLIB_DB_H_

#include "db/hashtable_db.h"

#include <string>
#include <vector>
#include "sitevm/sitevm.h"
#include "lib/itm_slib_hashtable.h"

namespace ycsbc {

template <class Alloc>
class ItmSlibDB : public HashtableDB {
 public:
  ItmSlibDB() : HashtableDB(Alloc::template New<vmp::ItmSlibHashtable<
      HashtableDB::FieldHashtable *, Alloc>>()) { }

  ~ItmSlibDB() {
    std::vector<KeyHashtable::KVPair> key_pairs = key_table_->Entries();
    for (auto &key_pair : key_pairs) {
      DeleteFieldHashtable(key_pair.second);
    }
    Alloc::Delete(key_table_);

    char *method = std::getenv("ITM_DEFAULT_METHOD");
    if (method && strncmp(method, "svm", 3) == 0) {
      sitevm::sitevm_shutdown();
    }
  }

  void Open() {
    Alloc::ThreadInit();
  }

  void Close() {
    Alloc::ThreadExit();
  }
 protected:
  HashtableDB::FieldHashtable *NewFieldHashtable() {
    return Alloc::template New<vmp::ItmSlibHashtable<const char *, Alloc>>();
  }

  void DeleteFieldHashtable(HashtableDB::FieldHashtable *table) {
    std::vector<FieldHashtable::KVPair> pairs = table->Entries();
    for (auto &pair : pairs) {
      DeleteString(pair.second);
    }
    Alloc::Delete((vmp::ItmSlibHashtable<const char *, Alloc> *)table);
  }

  const char *CopyString(const std::string &str) {
    void *value = Alloc::Malloc(str.length() + 1);
    memcpy(value, (const void *)str.c_str(), str.length() + 1);
    return (char *)value;
  }

  void DeleteString(const char *str) {
    Alloc::Free(str, strlen(str) + 1);
  }
};

} // ycsbc

#endif // YCSB_C_ITM_SLIB_DB_H_

