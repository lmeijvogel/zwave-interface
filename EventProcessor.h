#ifndef EventProcessor_H_
#define EventProcessor_H_
#include <stdlib.h>
#include "Common.h"
#include "LightsState.h"
#include "Node.h"

namespace MyZWave {
  class EventProcessor {
    public:
      EventProcessor();
      void ProcessEvent(NodeInfo *nodeInfo, uint8 event);

    private:
      void OnPressed();
      void OffPressed();

      bool IsNight();
      bool IsMorning();

      int GetHour();
      void TransitionTo(LightsState newState);

      std::map<LightsState,LightsState> masterSwitchOnTransitions;
      LightsState currentState;

      uint8 masterSwitchId;
      uint8 myOnlyLightId;
  };
}
#endif
