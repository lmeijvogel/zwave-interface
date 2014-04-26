#ifndef ManagerFactory_H_
#define ManagerFactory_H_
#include <string>
#include "Notification.h"

namespace MyZWave {
  class ManagerFactory {
    public:
      ManagerFactory(std::string port, void (*OnNotification)(OpenZWave::Notification const*, void*));
      void Create();
      void Destroy();

    private:
      std::string port;
      void (*OnNotification)(OpenZWave::Notification const*, void*);
  };
}
#endif
