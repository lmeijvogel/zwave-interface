#include "RedisListener.h"
#include <iostream>
#include <hiredis/hiredis.h>

namespace MyZWave {
  RedisListener::RedisListener(std::string channel, void (*resultHandler)(std::string input)) :
    channel_(channel),
    resultHandler_(resultHandler)
  {
  }

  void RedisListener::Start()
  {
    thread_ = boost::thread(&RedisListener::ListeningThread, this);
  }

  void RedisListener::ListeningThread()
  {
    redisContext *redis;
    try {
      redis = redisConnect("127.0.0.1", 6379);

      if (redis->err) {
        std::cout << "Error: " << redis->errstr << std::endl;
      }

      void *reply_alloc;

      reply_alloc = (void *)redisCommand(redis, ("subscribe "+channel_).c_str() );

      struct timeval tv = { 1, 1000 };

      for (int count = 0 ; ; count++) {
        redisSetTimeout(redis, tv);

        if (redisGetReply(redis, &reply_alloc) == REDIS_OK) {
          redisReply *reply = (redisReply *)reply_alloc;

          std::string message = reply->element[2]->str;
          resultHandler_(message);
          freeReplyObject(reply_alloc);
        }

        // If the error state is not cleared, subsequent redisGetReply
        // calls will fail
        redis->err = 0;

        // Allow interrupting the thread here.
        boost::this_thread::interruption_point();
      }
    }
    catch(boost::thread_interrupted&)
    {
      redisFree(redis);
      return;
    }
  }

  void RedisListener::Stop()
  {
    thread_.interrupt();
    thread_.join();
  }
}

