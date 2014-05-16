#ifndef RedisListener_H_
#define RedisListener_H_

#include <string>

namespace MyZWave {
  class RedisListener {
    public:
      RedisListener(std::string channel, void (*resultHandler)(std::string input));
      void Start();
      void Stop();

    private:
      std::string channel_;
      void (*resultHandler_)(std::string input);

      bool stopping_;
  };
}
#endif
