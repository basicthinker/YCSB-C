//
// A C++ Redis client that wraps hiredis
//

#ifndef YCSB_C_REDIS_CLIENT_H_
#define YCSB_C_REDIS_CLIENT_H_

#include <iostream>
#include <string>
#include "redis/hiredis/hiredis.h"

namespace ycsbc {

class RedisClient {
 public:
  RedisClient(const char *host, int port, bool sync);
  ~RedisClient();

  int Command(std::string cmd, int num_replicas = 3, int timeout_ms = 3000);

  redisContext *context() { return context_; }
 private:
  void HandleError(redisReply *reply, const char *hint);

  redisContext *context_;
  bool sync_;
};

//
// Implementation
//
inline RedisClient::RedisClient(const char *host, int port, bool sync) :
    sync_(sync) {
  context_ = redisConnect(host, port);
  if (!context_ || context_->err) {
    if (context_) {
      std::cerr << "Connect error: " << context_->errstr << std::endl;
      redisFree(context_);
    } else {
      std::cerr << "Connect error: can't allocate redis context!" << std::endl;
    }
    exit(1);
  }
  std::cerr << "Redis connected to " << host << ":" << port;
  std::cerr << " sync=" << sync << std::endl;
}

inline RedisClient::~RedisClient() {
  if (context_) {
    redisFree(context_);
  }
}

inline int RedisClient::Command(std::string cmd, int n, int t) {
  redisReply *reply;
  redisAppendCommand(context_, cmd.data());
  if (sync_) {
    redisAppendCommand(context_, "WAIT %d %d", n, t);
  }
  if (redisGetReply(context_, (void **)&reply) == REDIS_ERR) {
    HandleError(reply, cmd.c_str());
  }
  freeReplyObject(reply);
  if (sync_) {
    if (redisGetReply(context_, (void **)&reply) == REDIS_ERR) {
      HandleError(reply, "WAIT");
    }
    freeReplyObject(reply);
  }
  return 0;
}

inline void RedisClient::HandleError(redisReply *reply, const char *hint) {
  std::cerr << hint << " error: " << context_->errstr << std::endl;
  if (reply) freeReplyObject(reply);
  redisFree(context_);
  exit(2); 
}

} // namespace ycsbc

#endif // YCSB_C_REDIS_CLIENT_H_
