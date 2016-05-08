#! /bin/bash

REDIS=~/redis

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

./sample

$REDIS/src/redis-cli hmget Ren field1 field2

