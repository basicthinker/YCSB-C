//
//  svm_slib_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/svm_slib_db.h"

#include <vector>
#include "sitevm/sitevm.h"
#include "slib/mem_alloc.h"
#include "lib/slib_hashtable.h"
#include "lib/trans_def.h"

using vmp::SlibHashtable;
using slib::SvmAlloc;

namespace ycsbc {

SvmSlibDB::SvmSlibDB() : HashtableDB(NULL) {
  int err = sitevm_init();
  assert(!err);
  svm_ = sitevm_seg_create(NULL, SVM_SIZE);
  sitevm_malloc::init_sitevm_malloc(svm_);
  err = sitevm_enter();
  assert(!err);
  err = sitevm_open_and_update(svm_);
  assert(!err);
  key_table_ = SvmAlloc::New<
      SlibHashtable<HashtableDB::FieldHashtable *, SvmAlloc>>();
  err = sitevm_commit_and_update(svm_);
  assert(!err);
}

void SvmSlibDB::Init() {
  int err = sitevm_enter();
  assert(!err);
  err = sitevm_open_and_update(svm_);
  assert(!err);
}

void SvmSlibDB::Close() {
  sitevm_exit();
}

SvmSlibDB::~SvmSlibDB() {
  std::vector<KeyHashtable::KVPair> key_pairs = key_table_->Entries();
  for (auto &key_pair : key_pairs) {
    DeleteFieldHashtable(key_pair.second);
  }
  SvmAlloc::Delete(key_table_);
}

int SvmSlibDB::Read(const std::string &table, const std::string &key,
    const std::vector<std::string> *fields, std::vector<KVPair> &result) {
  int ret;
  TRANS_BEGIN {
    ret = HashtableDB::Read(table, key, fields, result);
  } TRANS_END(svm_);
  return ret;
}

int SvmSlibDB::Scan(const std::string &table, const std::string &key,
    int len, const std::vector<std::string> *fields,
    std::vector<std::vector<KVPair>> &result) {
  int ret;
  TRANS_BEGIN {
    ret = HashtableDB::Scan(table, key, len, fields, result);
  } TRANS_END(svm_);
  return ret;
}

int SvmSlibDB::Update(const std::string &table, const std::string &key,
    std::vector<KVPair> &values) {
  int ret;
  TRANS_BEGIN {
    ret = HashtableDB::Update(table, key, values);
  } TRANS_END(svm_);
  return ret;
}

int SvmSlibDB::Insert(const std::string &table, const std::string &key,
    std::vector<KVPair> &values) {
  int ret;
  TRANS_BEGIN {
    ret = HashtableDB::Insert(table, key, values);
  } TRANS_END(svm_);
  return ret;
}

int SvmSlibDB::Delete(const std::string &table, const std::string &key) {
  int ret;
  TRANS_BEGIN {
    ret = HashtableDB::Delete(table, key);
  } TRANS_END(svm_);
  return ret;
}

HashtableDB::FieldHashtable *SvmSlibDB::NewFieldHashtable() {
  return SvmAlloc::New<SlibHashtable<const char *, SvmAlloc>>();
}

void SvmSlibDB::DeleteFieldHashtable(HashtableDB::FieldHashtable *table) {
  std::vector<FieldHashtable::KVPair> pairs = table->Entries();
  for (auto &pair : pairs) {
    DeleteString(pair.second);
  }
  SvmAlloc::Delete(table);
}

const char *SvmSlibDB::CopyString(const std::string &str) {
  char *value = (char *)SvmAlloc::Malloc(str.length() + 1);
  strcpy(value, str.c_str());
  return value;
}

void SvmSlibDB::DeleteString(const char *str) {
  SvmAlloc::Free(str, strlen(str) + 1);
}

} // ycsbc

