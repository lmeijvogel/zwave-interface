#ifndef CommandParser_H_
#define CommandParser_H_

#include "LightsController.h"
#include "Programme.h"
#include "Light.h"

namespace MyZWave {
  class CommandParser {
    public:
      CommandParser(LightsController &lightsController);

      void ParseCommand(std::string input);
    private:
      LightsController &lightsController_;

      std::map<string,Programme> programmeTranslations_;
      std::map<string,Light> lightTranslations_;

      void NodeUnknownMessage(int nodeId);
      bool ParseProgramme(char *descriptor, Programme *programme);
  };
}
#endif
