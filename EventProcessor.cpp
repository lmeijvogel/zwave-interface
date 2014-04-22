#include "EventProcessor.h"
#include "MyNode.h"

/* Temporarily hardcode the state transitions and IDs.
 * I hope I'll be able to make them configurable.
 *
 * Don't do this via telnet, it will probably feel slower.
 */
namespace MyZWave {
  EventProcessor::EventProcessor(LightsController &lightsController) :
    lightsController(lightsController)
  {
    masterSwitchId = 3;

    masterSwitchOnTransitions[ Lights_Off           ] = Lights_Regular;

    masterSwitchOnTransitions[ Lights_Morning       ] = Lights_Morning;

    masterSwitchOnTransitions[ Lights_Dimmed        ] = Lights_Regular;
    masterSwitchOnTransitions[ Lights_Regular       ] = Lights_Dimmed;

    masterSwitchOnTransitions[ Lights_Night         ] = Lights_Night;

    masterSwitchOnTransitions[ Lights_Uninitialized ] = Lights_Regular;
    masterSwitchOnTransitions[ Lights_Custom        ] = Lights_Regular;
  }

  void EventProcessor::ProcessEvent(NodeInfo *nodeInfo, uint8 event) {
    if (nodeInfo->m_nodeId == masterSwitchId) {
      if (event == 0) {
        OffPressed();
      } else {
        OnPressed();
      }
    }
  }

  void EventProcessor::OnPressed() {
    Programme newState;
    if (IsNight()) {
      newState = Lights_Night;
    } else if (IsMorning()) {
      newState = Lights_Morning;
    } else {
      newState = masterSwitchOnTransitions[currentState];
    }

    TransitionTo(newState);
  }

  void EventProcessor::OffPressed() {
    TransitionTo(Lights_Off);
  }

  bool EventProcessor::IsNight() {
    int hour = GetHour();
    return (0 < hour && hour < 6);
  }

  bool EventProcessor::IsMorning() {
    int hour = GetHour();
    return (6 < hour && hour < 12);
  }

  int EventProcessor::GetHour() {
    struct tm * timeInfo;

    time_t currentTime = time(NULL);

    timeInfo = localtime(&currentTime);

    return timeInfo->tm_hour;
  }

  void EventProcessor::TransitionTo(Programme newState) {
    lightsController.SetProgramme(newState);
    currentState = newState;
  }
}
