#include "CommandParser.h"
#include "MyNode.h"
#include "Common.h"
#include "Manager.h"

#include <stdlib.h>
#include "boost/format.hpp"

namespace MyZWave {
  CommandParser::CommandParser(TelnetServer &telnetServer) :
    telnetServer_(telnetServer)
  {
  }

  void CommandParser::ParseCommand(std::string input) {
    int nodeId, classId, index, level;

    if (sscanf(input.c_str(), "set %i 0x%x %i %i\n", &nodeId, &classId, &index, &level) == 4) {
      NodeInfo *nodeInfo = MyZWave::MyNode::FindNodeById(nodeId);

      if (!nodeInfo || !nodeInfo->m_nodeId) {
        NodeUnknownMessage(nodeId);
        return;
      }

      string result;
      if (MyZWave::MyNode::SetValue(nodeInfo, classId, index, level)) {
        result = (boost::format("OK: %i 0x%x %i %i\n") % (int)nodeId % (int)classId % (int)index % (int)level).str();
      }
      else
      {
        result = (boost::format("ERROR: %i 0x%x %i %i\n") % (int)nodeId % (int)classId % (int)index % (int)level).str();
      }

      telnetServer_.WriteLine(result);

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

        telnetServer_.WriteLine(result);
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
      telnetServer_.WriteLine(message);

    }

    else {
      std::string message = "Unknown command!\n";

      telnetServer_.WriteLine(message);
    }
  }

  void CommandParser::NodeUnknownMessage(int nodeId) {
    string message = (boost::format("ERROR: Node %i unknown\n") % (int)nodeId).str();
    telnetServer_.WriteLine(message);
  }
}
