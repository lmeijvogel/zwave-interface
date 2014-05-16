#ifndef CommandParser_H_
#define CommandParser_H_

#include "TelnetServer.h"
#include "LightsController.h"
#include "Programme.h"

namespace MyZWave {
  class CommandParser {
    public:
      CommandParser(LightsController &lightsController);

      void ParseCommand(std::string input);
    private:
      //TelnetServer &telnetServer_;
      LightsController &lightsController_;

      std::map<string,Programme> programmeTranslations_;

      void NodeUnknownMessage(int nodeId);
      bool ParseProgramme(char *descriptor, Programme *programme);
  };
}
#endif
