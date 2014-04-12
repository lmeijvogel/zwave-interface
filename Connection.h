#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace MyZWave {
  class Connection {
    public:
    Connection(tcp::socket &socket);
    void ReadLine(boost::asio::streambuf &buffer, boost::system::error_code &error);
    void Close();

    private:
    tcp::socket &socket_;
  };
}
