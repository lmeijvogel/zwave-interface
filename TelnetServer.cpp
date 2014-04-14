#include "TelnetServer.h"

using boost::asio::ip::tcp;

namespace MyZWave {
  TelnetServer::TelnetServer(int port)
  : io_service_(),
    acceptor_(io_service_),
    port_(port),
    connectionInitialized_(false),
    connectionClosed_(false)
  {
  }

  int TelnetServer::listen(void (*resultHandler)(std::string input)) {
    try {
      boost::asio::ip::tcp::resolver resolver(io_service_);

      char portStr[16];
      sprintf(portStr, "%d", port_);

      // Only listen on loopback interface, I'm not smart enough to
      // get unix domain sockets working (or rather: getting the acceptor
      // to stop).
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", portStr );
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
      acceptor_.open(endpoint.protocol());
      acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      acceptor_.bind(endpoint);

      acceptor_.listen();

      while (!connectionClosed_)
      {
        socket_ = new tcp::socket(io_service_);
        acceptor_.accept(*socket_);

        connectionInitialized_ = true;

        boost::system::error_code error;

        boost::asio::streambuf buffer;

        int loop = true;
        while (loop) {
          ReadLine(buffer, error);

          if (connectionClosed_) {
            printf("Closed connection due to server shutdown\n");

            loop = false;
            break;
          }

          if (error.value() != 0) {
            printf("Error occurred: %i\n", error.value());

            loop = false;
            break;
          }

          std::istream str(&buffer);
          std::string line;
          std::getline(str, line);

          std::cout << line << std::endl;

          (*resultHandler)(line);
        }

        delete socket_;
        connectionInitialized_ = false;
      }
    } catch (std::exception& e)
    {
      std::cerr << "EXCEPTION!" << std::endl;
      std::cerr << e.what() << std::endl;

      return 1;
    }

    return 0;
  }

  void TelnetServer::ReadLine(boost::asio::streambuf &buffer, boost::system::error_code &error) {
    boost::asio::read_until(*socket_, buffer, '\n', error);
  }

  void TelnetServer::WriteLine(std::string &line) {
    long int length = line.length();
    boost::asio::write(*socket_, boost::asio::buffer(line, length));
  }

  void TelnetServer::Stop() {
    acceptor_.close();

    if (connectionInitialized_) {
      boost::system::error_code ec;
      socket_->shutdown(tcp::socket::shutdown_both, ec);
      socket_->close();
    }

    connectionClosed_ = true;
  }
}
