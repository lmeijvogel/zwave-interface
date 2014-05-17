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
    std::cout << "Creating thread" << std::endl;
    thread_ = boost::thread(&RedisListener::ListeningThread, this);
    std::cout << "... created" << std::endl;
  }

  void RedisListener::ListeningThread()
  {
    try {
      std::cout << "Thread started!" << std::endl;
      redisContext *redis = redisConnect("127.0.0.1", 6379);

      if (redis->err) {
        std::cout << "Error: " << redis->errstr << std::endl;
      }

      void *reply_alloc;

      reply_alloc = (void *)redisCommand(redis, ("subscribe "+channel_).c_str() );

      std::cout << "after reply_alloc\n";
      //stopping_ = false;

      while (redisGetReply(redis, &reply_alloc) == REDIS_OK) {
        std::cout << "inside while\n";
        redisReply *reply = (redisReply *)reply_alloc;

        std::string message = reply->element[2]->str;
        resultHandler_(message);
      }
    }
    catch(boost::thread_interrupted&)
    {
      std::cout << "Thread stopped" <<std::endl;
      return;
    }
  }

  void RedisListener::Stop()
  {
    std::cout << "Stopping!" << std::endl;
    thread_.interrupt();
    thread_.join();
  }
}

