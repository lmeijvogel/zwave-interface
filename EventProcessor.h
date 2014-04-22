#ifndef EventProcessor_H_
#define EventProcessor_H_
#include <stdlib.h>
#include "Common.h"
#include "LightsController.h"
#include "Programme.h"
#include "Node.h"

namespace MyZWave {
  class EventProcessor {
    public:
      EventProcessor(LightsController &lightsController);
      void ProcessEvent(NodeInfo *nodeInfo, uint8 event);

    private:
      void OnPressed();
      void OffPressed();

      bool IsNight();
      bool IsMorning();

      int GetHour();
      void TransitionTo(Programme newState);

      std::map<Programme,Programme> masterSwitchOnTransitions;
      Programme currentState;

      LightsController &lightsController;

      uint8 masterSwitchId;
      uint8 myOnlyLightId;
  };
}
#endif
