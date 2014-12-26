//
//  hashtable_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/24/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/hashtable_db.h"

#include <string>
#include <vector>
#include "lib/string_hashtable.h"

using std::string;
using std::vector;
using vmp::StringHashtable;

namespace ycsbc {

int HashtableDB::Read(const string &table, const string &key,
    const vector<string> *fields, vector<KVPair> &result) {
  string key_index(table + key);
  FieldHashtable *field_table = key_table_->Get(key_index.c_str());
  if (!field_table) return DB::kErrorNoData;

  result.clear();
  if (!fields) {
    vector<FieldHashtable::KVPair> field_pairs = field_table->Entries();
    for (auto &field_pair : field_pairs) {
      result.push_back(std::make_pair(field_pair.first, field_pair.second));
    }
  } else {
    for (auto &field : *fields) {
      const char *value = field_table->Get(field.c_str());
      if (!value) continue;
      result.push_back(std::make_pair(field, value));
    }
  }
  return DB::kOK;
}

int HashtableDB::Scan(const string &table, const string &key, int len,
    const vector<string> *fields, vector<vector<KVPair>> &result) {
  string key_index(table + key);
  vector<KeyHashtable::KVPair> key_pairs =
      key_table_->Entries(key_index.c_str(), len);

  result.clear();
  for (auto &key_pair : key_pairs) {
    FieldHashtable *field_table = key_pair.second;

    vector<KVPair> field_values;
    if (!fields) {
      vector<FieldHashtable::KVPair> field_pairs = field_table->Entries();
      for (auto &field_pair : field_pairs) {
        field_values.push_back(
            std::make_pair(field_pair.first, field_pair.second));
      }
    } else {
      for (auto &field : *fields) {
        const char *value = field_table->Get(field.c_str());
        if (!value) continue;
        field_values.push_back(std::make_pair(field, value));
      }
    }

    result.push_back(field_values);
  }
  return DB::kOK;
}

int HashtableDB::Update(const string &table, const string &key,
    vector<KVPair> &values) {
  string key_index(table + key);
  FieldHashtable *field_table = key_table_->Get(key_index.c_str());
  if (!field_table) {
    field_table = NewFieldHashtable();
    key_table_->Insert(key_index.c_str(), field_table);
    for (KVPair &field_pair : values) {
      const char *value = CopyString(field_pair.second);
      field_table->Insert(field_pair.first.c_str(), value);
    }
  } else {
    for (KVPair &field_pair : values) {
      const char *value = CopyString(field_pair.second);
      const char *old = field_table->Update(field_pair.first.c_str(), value);
      if (!old) {
        field_table->Insert(field_pair.first.c_str(), value);
      } else {
        DeleteString(old);
      }
    }
  }
  return DB::kOK;
}

int HashtableDB::Insert(const string &table, const string &key,
    vector<KVPair> &values) {
  string key_index(table + key);
  FieldHashtable *field_table = key_table_->Get(key_index.c_str());
  if (!field_table) {
    field_table = NewFieldHashtable();
    key_table_->Insert(key_index.c_str(), field_table);
  }

  for (KVPair &field_pair : values) {
    const char *value = CopyString(field_pair.second);
    bool ok = field_table->Insert(field_pair.first.c_str(), value);
    if (!ok) {
      DeleteString(value);
      return DB::kErrorConflict;
    }
  }
  return DB::kOK;
}

int HashtableDB::Delete(const string &table, const string &key) {
  string key_index(table + key);
  FieldHashtable *field_table = key_table_->Remove(key_index.c_str());
  if (!field_table) {
    return DB::kErrorNoData;
  } else {
    DeleteFieldHashtable(field_table);
  }
  return DB::kOK;
}

} // ycsbc
