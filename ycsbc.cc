//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <future>
#include "utils.h"
#include "timer.h"
#include "client.h"
#include "core_workload.h"
#include "db_factory.h"

using namespace std;

void UsageMessage(const char *command);
bool StrStartWith(const char *str, const char *pre);
void ParseCommandLine(int argc, const char *argv[], utils::Properties &props);

double DelegateClient(ycsbc::DB *db, ycsbc::CoreWorkload *wl, const int num_ops,
    bool is_loading) {
  ycsbc::Client client(*db, *wl);
  utils::Timer<double> timer;
  timer.Start();
  for (int i = 0; i < num_ops; ++i) {
    if (is_loading) {
      client.DoInsert();
    } else {
      client.DoTransaction();
    }
  }
  return timer.End();
}

int main(const int argc, const char *argv[]) {
  utils::Properties props;
  ParseCommandLine(argc, argv, props);

  ycsbc::DB *db = ycsbc::DBFactory::CreateDB(props["dbname"]);
  if (!db) {
    cout << "Unknown database name " << props["dbname"] << endl;
    exit(0);
  }
  db->Init(props);

  ycsbc::CoreWorkload wl;
  wl.Init(props);

  const int num_threads = stoi(props["threadcount"]);

  // Loads data
  vector<future<double>> times;
  int num_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
  for (int i = 0; i < num_threads; ++i) {
    times.emplace_back(
        async(launch::async, DelegateClient, db, &wl, num_ops, true));
  }
  assert((int)times.size() == num_threads);

  double sum = 0.0;
  for (auto &t : times) {
    assert(t.valid());
    sum += t.get();
  }
  cout << "Loading throughput\t" << num_ops * num_threads / sum << endl;

  // Peforms transactions
  times.clear();
  num_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
  for (int i = 0; i < num_threads; ++i) {
    times.emplace_back(
        async(launch::async, DelegateClient, db, &wl, num_ops, false));
  }
  assert((int)times.size() == num_threads);

  sum = 0.0;
  for (auto &t : times) {
    assert(t.valid());
    sum += t.get();
  }
  cout << "Transaction throughput\t" << num_ops * num_threads / sum << endl;
}

void ParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
  int argindex = 1;
  while (argindex < argc && StrStartWith(argv[argindex], "-")) {
    if (strcmp(argv[argindex], "-threads") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("threadcount", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-db") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbname", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-P") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }

      ifstream input(argv[argindex]);
      try {
        props.Load(input);
      } catch (const string &message) {
        cout << message << endl;
        exit(0);
      }
      input.close();
      argindex++;
    } else {
      cout << "Unknown option " << argv[argindex] << endl;
      exit(0);
    }
  }

  if (argindex == 1 || argindex != argc) {
    UsageMessage(argv[0]);
    exit(0);
  }
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

