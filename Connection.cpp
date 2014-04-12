#include "Connection.h"

using boost::asio::ip::tcp;

namespace MyZWave {
  Connection::Connection(tcp::socket &socket) :
    socket_(socket)
  {}

  void Connection::ReadLine(boost::asio::streambuf &buffer, boost::system::error_code &error) {
    boost::asio::read_until(socket_, buffer, '\n', error);
  }

  void Connection::Close() {
    boost::system::error_code ec;
    socket_.shutdown(tcp::socket::shutdown_both, ec);
    socket_.close();
  }

};
