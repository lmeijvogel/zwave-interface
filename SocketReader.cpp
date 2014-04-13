#include "SocketReader.h"

using boost::asio::ip::tcp;

namespace MyZWave {
  SocketReader::SocketReader(int port)
  : io_service_(),
    acceptor_(io_service_),
    port_(port),
    connectionInitialized_(false)
  {
  }

  int SocketReader::listen(void (*resultHandler)(std::string input)) {
    try {
      boost::asio::ip::tcp::resolver resolver(io_service_);

      // Only listen on loopback interface, I'm not smart enough to
      // get unix domain sockets working (or rather: getting the acceptor
      // to stop).
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", "2014");
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
      acceptor_.open(endpoint.protocol());
      acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      acceptor_.bind(endpoint);

      acceptor_.listen();

      for (;;)
      {
        tcp::socket socket(io_service_);
        acceptor_.accept(socket);

        connectionInitialized_ = true;
        connection_ = new Connection(socket);
        // TODO: Save the socket so any connections can be closed.
        int loop = true;

        printf("Accepted connection!\n");
        boost::system::error_code error;

        boost::asio::streambuf buffer;

        while (loop) {
          printf("reading\n");
          connection_->ReadLine(buffer, error);

          if (error == boost::asio::error::eof) {
            printf("EOF!\n");
            loop = false;
            break;
          }

          std::istream str(&buffer);
          std::string line;
          std::getline(str, line);

          (*resultHandler)(line);
          printf("Line: %s\n", line.c_str());
        }
      }
    } catch (std::exception& e)
    {
      std::cerr << "EXCEPTION!" << std::endl;
      std::cerr << e.what() << std::endl;

      return 1;
    }

    return 0;
  }

  void SocketReader::WriteLine(std::string &line) {
    connection_->WriteLine(line);
  }

  void SocketReader::stop() {
    acceptor_.close();

    if (connectionInitialized_) {
      connection_->Close();
    }
  }
}
