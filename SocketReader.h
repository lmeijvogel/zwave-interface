#ifndef SocketReader_H_
#define SocketReader_H_

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "Connection.h"

using boost::asio::ip::tcp;

namespace MyZWave {
  class SocketReader {
    public:
      SocketReader(int port);

      int listen( void (*resultHandler)(std::string input));
      void WriteLine(std::string &line);
      void stop();

    private:
      boost::asio::io_service io_service_;
      boost::asio::ip::tcp::acceptor acceptor_;
      int port_;

      Connection *connection_;
      bool connectionInitialized_;
  };
}
#endif

