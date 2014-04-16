#include "EventProcessor.h"
#include "MyNode.h"

/* Temporarily hardcode the state transitions and IDs.
 * I hope I'll be able to make them configurable.
 *
 * Don't do this via telnet, it will probably feel slower.
 */
namespace MyZWave {
  EventProcessor::EventProcessor() {
    masterSwitchId = 3;
    myOnlyLightId = 2;

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
    LightsState newState;
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

  void EventProcessor::TransitionTo(LightsState newState) {
    // My only light. For now.
    NodeInfo *light = MyZWave::MyNode::FindNodeById(myOnlyLightId);

    uint8 intensity;

    switch (newState) {
      case Lights_Off:     intensity =  0; break;
      case Lights_Morning: intensity = 80; break;
      case Lights_Regular: intensity = 99; break;
      case Lights_Dimmed:  intensity = 30; break;
      case Lights_Night:   intensity = 10; break;
      default:             intensity =  0; break;
    }

    MyZWave::MyNode::SetValue(light, 0x26, 0, intensity);
    currentState = newState;
  }
}
