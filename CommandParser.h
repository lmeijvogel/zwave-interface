#ifndef CommandParser_H_
#define CommandParser_H_

#include "TelnetServer.h"

namespace MyZWave {
  class CommandParser {
    public:
      CommandParser(TelnetServer &telnetServer);

      void ParseCommand(std::string input);
    private:
      TelnetServer &telnetServer_;
      void NodeUnknownMessage(int nodeId);
  };
}
#endif
