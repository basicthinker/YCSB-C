//
//  redis_db.cc
//  YCSB-C
//

#include "redis_db.h"

#include <cstring>

using namespace std;

namespace ycsbc {

int RedisDB::Read(const string &table, const string &key,
         const vector<string> *fields,
         vector<KVPair> &result) {
  if (fields) {
    int argc = fields->size() + 2;
    const char *argv[argc];
    size_t argvlen[argc];
    int i = 0;
    argv[i] = "HMGET"; argvlen[i] = strlen(argv[i]);
    argv[++i] = key.c_str(); argvlen[i] = key.length();
    for (const string &f : *fields) {
      argv[++i] = f.data(); argvlen[i] = f.size();
    }
    assert(i == argc - 1);
    redisReply *reply = (redisReply *)redisCommandArgv(
        redis_.context(), argc, argv, argvlen);
    if (!reply) return DB::kOK;
    assert(reply->type == REDIS_REPLY_ARRAY);
    assert(fields->size() == reply->elements);
    for (size_t i = 0; i < reply->elements; ++i) {
      const char *value = reply->element[i]->str;
      result.push_back(make_pair(fields->at(i), string(value ? value : "")));
    }
    freeReplyObject(reply);
  } else {
    redisReply *reply = (redisReply *)redisCommand(redis_.context(),
        "HGETALL %s", key.c_str());
    if (!reply) return DB::kOK;
    assert(reply->type == REDIS_REPLY_ARRAY);
    for (size_t i = 0; i < reply->elements / 2; ++i) {
      result.push_back(make_pair(
          string(reply->element[2 * i]->str),
          string(reply->element[2 * i + 1]->str)));
    }
    freeReplyObject(reply);
  }
  return DB::kOK;
}

int RedisDB::Update(const string &table, const string &key,
           vector<KVPair> &values) {
  string cmd("HMSET");
  size_t len = cmd.length() + key.length() + 1;
  for (KVPair &p : values) {
    len += p.first.length() + p.second.length() + 2;
  }
  cmd.reserve(len);

  cmd.append(" ").append(key);
  for (KVPair &p : values) {
    assert(p.first.find(' ') == string::npos);
    cmd.append(" ").append(p.first);
    assert(p.second.find(' ') == string::npos);
    cmd.append(" ").append(p.second);
  }
  assert(cmd.length() == len);
  redis_.Command(cmd);
  return DB::kOK;
}

} // namespace ycsbc
