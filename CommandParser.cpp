#include "CommandParser.h"
#include "MyNode.h"
#include "Common.h"
#include "Manager.h"

#include <stdlib.h>
#include "boost/format.hpp"

namespace MyZWave {
  CommandParser::CommandParser(LightsController &lightsController) :
    lightsController_(lightsController)
  {
    programmeTranslations_["off"] = Lights_Off;
    programmeTranslations_["morning"] = Lights_Morning;
    programmeTranslations_["regular"] = Lights_Regular;
    programmeTranslations_["dimmed"] = Lights_Dimmed;
    programmeTranslations_["night"] = Lights_Night;

    lightTranslations_["kitchen"] = Light_Kitchen;
    lightTranslations_["uplight"] = Light_Uplight;
    lightTranslations_["coffeeTable"] = Light_CoffeeTable;
    lightTranslations_["diningTable"] = Light_DiningTable;

  }

  void CommandParser::ParseCommand(std::string input) {
    int nodeId, classId, index, level;
    char *lightName;
    char *programmeName;
    char *onOff;

    if (sscanf(input.c_str(), "dim %ms %i\n", &lightName, &level) == 2) {
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById((uint8)lightTranslations_[lightName]);

      bool  success = MyZWave::MyNode::SetValue(nodeInfo, 0x26, 0, level);
      string result;

      if (success) {
        result = "OK\n";
      }
      else {
        result = "Could not set value!\n";
      }

      cout << result;
      free(lightName);
    }
    else if (sscanf(input.c_str(), "switch %ms %ms\n", &lightName, &onOff)) {
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById((uint8)lightTranslations_[lightName]);

      bool success = MyZWave::MyNode::SetValue(nodeInfo, 0x25, 0, strcmp(onOff, "on") == 0);

      string result;

      if (success) {
        result = "OK\n";
      }
      else {
        result = "Could not set value!\n";
      }

      cout << result;
      free(lightName);
      free(onOff);
    }
    else if (sscanf(input.c_str(), "set %i 1x%x %i %i\n", &nodeId, &classId, &index, &level) == 4) {
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById(nodeId);

      if (!nodeInfo || !nodeInfo->m_nodeId) {
        NodeUnknownMessage(nodeId);
        return;
      }

      string result;
      bool success = false;

      if (classId == 0x25) { // switch, so boolean
        success = MyZWave::MyNode::SetValue(nodeInfo, classId, index, (level != 0));
        if (success) {
          result = "OK\n";
        }
      } else {
        success = MyZWave::MyNode::SetValue(nodeInfo, classId, index, level);
        if (success) {
          result = "OK\n";
        }
      }

      if (!success)
      {
        result = (boost::format("ERROR: %i 0x%x %i %i\n") % (int)nodeId % (int)classId % (int)index % (int)level).str();
      }

      std::cout << (result);

    }
    else if (sscanf(input.c_str(), "get %i 0x%x %i\n", &nodeId, &classId, &index) == 3) {
      uint8 value;
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById(nodeId);

      if (!nodeInfo || !nodeInfo->m_nodeId) {
        NodeUnknownMessage(nodeId);
        return;
      }

      if (MyZWave::MyNode::GetValue(nodeInfo, classId, index, &value)) {
        std::string result = (boost::format("%i 0x%x %i: %i\n") % (int)nodeId % (int)classId % (int)index % (int)value).str();

        std::cout << (result);
      }
    }
    else if (sscanf(input.c_str(), "refresh %i\n", &nodeId) == 1) {
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById(nodeId);

      if (!nodeInfo || !nodeInfo->m_nodeId) {
        NodeUnknownMessage(nodeId);
        return;
      }

      OpenZWave::Manager::Get()->TestNetworkNode(MyZWave::g_homeId, nodeId, 5);
      OpenZWave::Manager::Get()->RefreshNodeInfo(MyZWave::g_homeId, nodeId);
      string message = (boost::format("OK: Refreshing node %i\n") % (int)nodeId).str();
      std::cout << message << std::endl;

    }
    else if (sscanf(input.c_str(), "programme %ms\n", &programmeName) == 1) {
      Programme programme;

      if (ParseProgramme(programmeName, &programme)) {
        lightsController_.SetProgramme(programme);
      } else {
        string message = "Unknown programme!\n";
        std::cout << message << std::endl;
      }

      free(programmeName);
    }
    else {
      std::string message = "Unknown command!\n";

      std::cout << message << std::endl;
    }
  }

  void CommandParser::NodeUnknownMessage(int nodeId) {
    string message = (boost::format("ERROR: Node %i unknown\n") % (int)nodeId).str();
    std::cout << message << std::endl;
  }

  bool CommandParser::ParseProgramme(char *descriptor, Programme *state) {
    map<string,Programme>::iterator it = programmeTranslations_.find(descriptor);

    if (it != programmeTranslations_.end()) {
      *state = it->second;
      return true;
    }

    return false;
  }
}
