#include "RedisListener.h"
#include <iostream>
#include <hiredis/hiredis.h>

namespace MyZWave {
  RedisListener::RedisListener(std::string channel, void (*resultHandler)(std::string input)) :
    channel_(channel),
    resultHandler_(resultHandler)
  { }

  void RedisListener::Start()
  {
    // Example implementation.
    //
    redisContext *redis = redisConnect("127.0.0.1", 6379);

    if (redis->err) {
      std::cout << "Error: " << redis->errstr << std::endl;
    }

    void *reply_alloc;

    reply_alloc = (void *)redisCommand(redis, ("subscribe "+channel_).c_str() );

    stopping_ = false;

    while (!stopping_ && redisGetReply(redis, &reply_alloc) == REDIS_OK) {
      redisReply *reply = (redisReply *)reply_alloc;

      std::string message = reply->element[2]->str;
      resultHandler_(message);
    }
  }

  void RedisListener::Stop()
  {
    stopping_ = true;
  }
}

