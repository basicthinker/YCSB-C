//
//  aerospike_db.h
//  YCSB-C
//
//  Created by Jinglei Ren on 4/13/14.
//  Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_AEROSPIKE_DB_H_
#define YCSB_C_AEROSPIKE_DB_H_

#include "core/db.h"

#include <string>
#include <iostream>
#include <aerospike/aerospike.h>
#include <aerospike/aerospike_key.h>
#include "core/properties.h"

namespace ycsbc {

class AerospikeDB : public DB {
 public:
  AerospikeDB() : namespace_("test"), set_("YCSB") { }

  void Open(const char *host, uint16_t port) {
    as_config config;
    as_config_init(&config);
    config.hosts[0] = { .addr = host, .port = port };
    aerospike_init(&as_client_, &config);

    as_error err;
    if (aerospike_connect(&as_client_, &err) != AEROSPIKE_OK) {
      std::cerr << "Aeospike connect error: " << err.message << std::endl;
      aerospike_destroy(&as_client_);
      exit(EXIT_FAILURE);
    }
  }

  ~AerospikeDB() {
    as_error err;
    if (aerospike_close(&as_client_, &err) != AEROSPIKE_OK) {
      std::cerr << "Aeospike close error: " << err.message << std::endl;
    }
    aerospike_destroy(&as_client_);
  }

  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields,
           std::vector<KVPair> &result) {
    as_key k;
    as_key_init(&k, namespace_.c_str(), set_.c_str(), key.c_str());
    as_record *rec = nullptr;
    if (fields) {
      const char *bins[fields->size() + 1];
      int i = 0;
      for (auto &f : *fields) {
        bins[i] = f.c_str();
        ++i;
      }
      bins[i] = nullptr;

      as_error err;
      if (aerospike_key_select(&as_client_, &err, nullptr, &k, bins, &rec)
          != AEROSPIKE_OK) {
        std::cerr << "Aeospike select error: " << err.message << std::endl;
        return err.code;
      }
    } else {
      as_error err;
      if (aerospike_key_get(&as_client_, &err, nullptr, &k, &rec)
          != AEROSPIKE_OK) {
        std::cerr << "Aeospike get error: " << err.message << std::endl;
        return err.code;
      }
    }
    as_record_destroy(rec);
    return DB::kOK;
  }

  int Scan(const std::string &table, const std::string &key,
           int len, const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result) {
    return -1;
  }

  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values) {
    as_key k;
    as_key_init(&k, namespace_.c_str(), set_.c_str(), key.c_str());

    as_record rec;
    as_record_inita(&rec, values.size());
    for (auto &v : values) {
      as_record_set_str(&rec, v.first.c_str(), v.second.c_str());
    }

    as_error err;
    if (aerospike_key_put(&as_client_, &err, nullptr, &k, &rec)
        != AEROSPIKE_OK) {
      std::cerr << "Aeospike put error: " << err.message << std::endl;
      return err.code;
    }
    as_record_destroy(&rec);
    return DB::kOK;
  }

  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values) {
    return Update(table, key, values);
  }

  int Delete(const std::string &table, const std::string &key) {
    as_key k;
    as_key_init(&k, namespace_.c_str(), set_.c_str(), key.c_str());
    as_error err;
    if (aerospike_key_remove(&as_client_, &err, nullptr, &k) != AEROSPIKE_OK) {
      std::cerr << "Aeospike remove error: " << err.message << std::endl;
      return err.code;
    }
    return DB::kOK;
  }

 private:
  aerospike as_client_;

  std::string namespace_;
  std::string set_;
};

} // ycsbc

#endif // YCSB_C_AEROSPIKE_DB_H_

