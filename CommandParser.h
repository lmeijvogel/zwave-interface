#ifndef CommandParser_H_
#define CommandParser_H_

#include "TelnetServer.h"
#include "LightsController.h"
#include "LightsState.h"

namespace MyZWave {
  class CommandParser {
    public:
      CommandParser(TelnetServer &telnetServer, LightsController &lightsController);

      void ParseCommand(std::string input);
    private:
      TelnetServer &telnetServer_;
      LightsController &lightsController_;

      std::map<string,LightsState> programmeTranslations_;

      void NodeUnknownMessage(int nodeId);
      bool ParseProgramme(char *descriptor, LightsState *state);
  };
}
#endif
