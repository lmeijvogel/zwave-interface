#ifndef CommandParser_H_
#define CommandParser_H_

#include "EventProcessor.h"
#include "Programme.h"
#include "Light.h"

namespace MyZWave {
  class CommandParser {
    public:
      CommandParser(EventProcessor &eventProcessor);

      void ParseCommand(std::string input);
    private:
      EventProcessor &eventProcessor_;

      std::map<string,Programme> programmeTranslations_;
      std::map<string,Light> lightTranslations_;

      void NodeUnknownMessage(int nodeId);
      bool ParseProgramme(char *descriptor, Programme *programme);
  };
}
#endif
