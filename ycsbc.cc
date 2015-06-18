//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include <cstring>
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <vector>
#include "sitevm/sitevm.h"
#include "slib/resident_thread.h"
#include "core/client.h"
#include "core/core_workload.h"
#include "core/utils.h"
#include "db/db_factory.h"

using namespace std;
using namespace std::chrono;

void UsageMessage(const char *command);
bool StrStartWith(const char *str, const char *pre);
string ParseCommandLine(int argc, const char *argv[], utils::Properties &props);

alignas(64) std::atomic_uint_fast64_t g_total_num(0);
alignas(64) uint64_t n1, n2;
high_resolution_clock::time_point t1, t2;

int DelegateClient(ycsbc::DB *db, ycsbc::CoreWorkload *wl, bool is_loading) {
  char *name = getenv("DB_NAME");
  assert(name);
  char *method;
  if (strcmp(name, "itm_slib") == 0 &&
      (method = getenv("ITM_DEFAULT_METHOD")) &&
      strcmp(method, "svm") == 0) {
    int err = sitevm::sitevm_enter();
    assert(!err);
  }

  ycsbc::Client client(*db, *wl);
  int oks = 0;
  while (true) {
    uint64_t seq = g_total_num++;
    if (seq == n1) t1 = high_resolution_clock::now();
    else if (seq == n2) t2 = high_resolution_clock::now();
    else if (seq > n2) break;

    if (is_loading) {
      oks += client.DoInsert();
    } else {
     oks += client.DoTransaction();
    }
  }
  return oks;
}

int main(const int argc, const char *argv[]) {
  utils::Properties props;
  string file_name = ParseCommandLine(argc, argv, props);

  ycsbc::DB *db = ycsbc::DBFactory::CreateDB(props["dbname"]);
  if (!db) {
    cout << "Unknown database name " << props["dbname"] << endl;
    exit(EXIT_FAILURE);
  }
  setenv("DB_NAME", props["dbname"].c_str(), 1);

  ycsbc::CoreWorkload wl;
  wl.Init(props);

  const int num_threads = stoi(props["threadcount"]);
  slib::ResidentThread threads[num_threads];

  // Loads data
  vector<future<int>> actual_ops;
  n1 = 0;
  n2 = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
  for (auto &t : threads) {
    actual_ops.emplace_back(t.Run(
        DelegateClient, db, &wl, true));
  }
  assert((int)actual_ops.size() == num_threads);

  int sum = 0;
  for (auto &n : actual_ops) {
    assert(n.valid());
    sum += n.get();
  }
  cerr << "# Loading records:\t" << sum << endl;
 
  char *method;
  if (props["dbname"] == "itm_slib" &&
      (method = getenv("ITM_DEFAULT_METHOD")) && strcmp(method, "svm") == 0) {
    sitevm::sitevm_sync();
  }

  // Peforms transactions
  actual_ops.clear();
  int total_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
  n1 = 0.1 * total_ops;
  n2 = 0.9 * total_ops;
  for (auto &t : threads) {
    actual_ops.emplace_back(t.Run(
        DelegateClient, db, &wl, false));
  }
  assert((int)actual_ops.size() == num_threads);

  sum = 0;
  for (auto &n : actual_ops) {
    assert(n.valid());
    sum += n.get();
  }

  int64_t nsec = duration_cast<nanoseconds>(t2 - t1).count();
  double thr = (n2 - n1) * 1000000.0 / nsec; // KTPS
  uint64_t latency = num_threads * nsec / (n2 - n1) / 1000; // usec

  cerr << "# Transaction throughput (KTPS)" << endl;
  cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
  cerr << thr << '\t' << latency << endl;

  //ycsbc::DBFactory::DestroyDB(db);

  for (auto &t : threads) {
    t.Terminate();
  }

  if (props["dbname"] == "itm_slib" &&
      (method = getenv("ITM_DEFAULT_METHOD")) && strcmp(method, "svm") == 0) {
    sitevm::sitevm_shutdown();
  }
}

string ParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
  int argindex = 1;
  string filename;
  while (argindex < argc && StrStartWith(argv[argindex], "-")) {
    if (strcmp(argv[argindex], "-threads") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(EXIT_FAILURE);
      }
      props.SetProperty("threadcount", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-db") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(EXIT_FAILURE);
      }
      props.SetProperty("dbname", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-P") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(EXIT_FAILURE);
      }
      filename.assign(argv[argindex]);
      ifstream input(argv[argindex]);
      try {
        props.Load(input);
      } catch (const string &message) {
        cout << message << endl;
        exit(EXIT_FAILURE);
      }
      input.close();
      argindex++;
    } else {
      cout << "Unknown option " << argv[argindex] << endl;
      exit(EXIT_FAILURE);
    }
  }

  if (argindex == 1 || argindex != argc) {
    UsageMessage(argv[0]);
    exit(EXIT_FAILURE);
  }

  return filename;
}

void UsageMessage(const char *command) {
  cout << "Usage: " << command << " [options]" << endl;
  cout << "Options:" << endl;
  cout << "  -threads n: execute using n threads (default: 1)" << endl;
  cout << "  -db dbname: specify the name of the DB to use (default: basic)" << endl;
  cout << "  -P propertyfile: load properties from the given file. Multiple files can" << endl;
  cout << "                   be specified, and will be processed in the order specified" << endl;
}

inline bool StrStartWith(const char *str, const char *pre) {
  return strncmp(str, pre, strlen(pre)) == 0;
}

