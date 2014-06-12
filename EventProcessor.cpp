#include "EventProcessor.h"
#include "MyNode.h"

/* Temporarily hardcode the state transitions and IDs.
 * I hope I'll be able to make them configurable.
 *
 * Don't do this via telnet, it will probably feel slower.
 */
namespace MyZWave {
  EventProcessor::EventProcessor(LightsController &lightsController, TimeService &timeService) :
    lightsController(lightsController),
    timeService(timeService)
  {
    masterSwitchId = 3;

    masterSwitchOnTransitions[ Programme_Off           ] = Programme_Regular;

    masterSwitchOnTransitions[ Programme_Morning       ] = Programme_Morning;

    masterSwitchOnTransitions[ Programme_Dimmed        ] = Programme_Regular;
    masterSwitchOnTransitions[ Programme_Regular       ] = Programme_Dimmed;

    masterSwitchOnTransitions[ Programme_Night         ] = Programme_Night;

    masterSwitchOnTransitions[ Programme_Uninitialized ] = Programme_Regular;
    masterSwitchOnTransitions[ Programme_Custom        ] = Programme_Regular;
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
    if (timeService.IsNight()) {
      newState = Programme_Night;
    } else if (timeService.IsMorning()) {
      newState = Programme_Morning;
    } else {
      newState = masterSwitchOnTransitions[currentState];
    }

    TransitionTo(newState);
  }

  void EventProcessor::OffPressed() {
    TransitionTo(Programme_Off);
  }

  void EventProcessor::DirectTransition(Programme newState) {
    TransitionTo(newState);
  }

  void EventProcessor::TransitionTo(Programme newState) {
    lightsController.SetProgramme(newState);
    currentState = newState;
  }
}
