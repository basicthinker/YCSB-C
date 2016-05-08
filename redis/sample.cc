#include "redis_client.h"

using namespace ycsbc;

int main(int argc, const char *argv[]) {
  const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
  int port = (argc > 2) ? atoi(argv[2]) : 6379;

  RedisClient client(host, port, true);

  client.Command("HMSET Ren field1 jinglei@ren.systems field2 Jinglei");

  return 0;
}
