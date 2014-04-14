#ifndef TelnetServer_H_
#define TelnetServer_H_

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace MyZWave {
  class TelnetServer {
    public:
      TelnetServer(int port);

      int listen( void (*resultHandler)(std::string input));
      void ReadLine(boost::asio::streambuf &buffer, boost::system::error_code &error);
      void WriteLine(std::string &line);
      void Stop();

    private:
      boost::asio::io_service io_service_;
      boost::asio::ip::tcp::acceptor acceptor_;
      int port_;

      tcp::socket *socket_;
      bool connectionInitialized_;
      bool connectionClosed_;
  };
}
#endif

