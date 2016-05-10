#include <iostream>
#include <string>
#include <vector>
#include "redis/redis_client.h"
#include "db/redis_db.h"

using namespace std;
using namespace ycsbc;

int main(int argc, const char *argv[]) {
  const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
  int port = (argc > 2) ? atoi(argv[2]) : 6379;

  RedisClient client(host, port, 0);

  client.Command("HMSET Ren field1 jinglei@ren.systems field2 Jinglei");

  RedisDB db(host, port, false);
  db.Init();
  string key = "Ren";
  vector<string> fields;
  fields.push_back("field1");
  fields.push_back("field3");
  vector<DB::KVPair> result;

  db.Read(key, key, &fields, result);
  for (auto &p : result) {
    cout << p.first << '\t' << p.second << endl;
  }

  result[1].second = "HelloWorld!";
  db.Update(key, key, result);

  result.clear();
  db.Read(key, key, nullptr, result);
   for (auto &p : result) {
    cout << p.first << '\t' << p.second << endl;
  }

  db.Delete(key, key);
  result.clear();
  db.Read(key, key, nullptr, result);
  cout << "After delete: " << result.size() << endl;
  return 0;
}
