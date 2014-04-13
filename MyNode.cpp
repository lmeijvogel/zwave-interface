#include "MyNode.h"
#include <stdlib.h>
#include "Manager.h"

using namespace std;

namespace MyZWave {
  NodeInfo *MyNode::FindNodeById(uint8 id) {
    for( list<NodeInfo*>::iterator it = nodes.begin(); it != nodes.end(); ++it )
    {
      NodeInfo* nodeInfo = *it;

      // skip the controller (most likely node 1)
      if( nodeInfo->m_nodeId == 1) continue;
      if( nodeInfo->m_nodeId == id) {
        return nodeInfo;
      }
    }

    return NULL;
  }

  bool MyNode::GetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 *value) {
    for( list<OpenZWave::ValueID>::iterator it2 = nodeInfo->m_values.begin(); it2 != nodeInfo->m_values.end(); ++it2 )
    {
      OpenZWave::ValueID v = *it2;
      if (v.GetCommandClassId() == classId && v.GetIndex() == index) {
        OpenZWave::Manager::Get()->GetValueAsByte(v, value);

        return true;
      }
    }
    return false;
  }

  bool MyNode::SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 value) {
    for( list<OpenZWave::ValueID>::iterator it2 = nodeInfo->m_values.begin(); it2 != nodeInfo->m_values.end(); ++it2 )
    {
      OpenZWave::ValueID v = *it2;
      if (v.GetCommandClassId() == classId && v.GetIndex() == index) {
        printf("SetValue: (0x%x, 0x%x)\n", v.GetCommandClassId(), v.GetIndex());
        OpenZWave::Manager::Get()->SetValue(v, value);
        return true;
      }
    }
    return false;
  }

  void MyNode::PrintNode(NodeInfo *nodeInfo) {
    for( list<OpenZWave::ValueID>::iterator it2 = nodeInfo->m_values.begin(); it2 != nodeInfo->m_values.end(); ++it2 )
    {
      OpenZWave::ValueID v = *it2;

      PrintValueID(v);
    }
  }

  void MyNode::PrintValueID(OpenZWave::ValueID v) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    string label = manager->GetValueLabel(v);
    NodeInfo *nodeInfo = FindNodeById(v.GetNodeId());

    switch (v.GetType()) {
      case OpenZWave::ValueID::ValueType_Bool:
        bool bool_value;
        manager->GetValueAsBool(v, &bool_value);

        printf("%i(0x%x:%i): %s: %i\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str(), bool_value);
        break;
      case OpenZWave::ValueID::ValueType_Byte:
        uint8 byte_value, actual_value;
        manager->GetValueAsByte(v, &byte_value);

        GetValue(nodeInfo, v.GetCommandClassId(), v.GetIndex(), &actual_value);

        printf("%i(0x%x:%i): %s: %i (%i)\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str(), byte_value, actual_value);
        break;
      case OpenZWave::ValueID::ValueType_Decimal:
        float decimal_value;
        manager->GetValueAsFloat(v, &decimal_value);

        printf("%i(0x%x:%i): %s: %f\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str(), decimal_value);
        break;
      case OpenZWave::ValueID::ValueType_Int:
        int32 int_value;
        manager->GetValueAsInt(v, &int_value);

        printf("%i(0x%x:%i): %s: %i\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str(), int_value);
        break;
      case OpenZWave::ValueID::ValueType_List:
        // No support yet, does not seem necessary
        printf("%i(0x%x:%i): %s: LIST\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str());
        break;
      case OpenZWave::ValueID::ValueType_Schedule:
        // No support yet, does not seem necessary
        printf("%i(0x%x:%i): %s: SCHEDULE\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str());
        break;
      case OpenZWave::ValueID::ValueType_Short:
        int16 short_value;
        manager->GetValueAsShort(v, &short_value);

        printf("%i(0x%x:%i): %s: %i\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str(), short_value);
        break;
      case OpenZWave::ValueID::ValueType_String:
        // No support yet, does not seem necessary
        printf("%i(0x%x:%i): %s: STRING\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str());
        break;
      case OpenZWave::ValueID::ValueType_Button:
        // No support yet, does not seem necessary
        printf("%i(0x%x:%i): %s: BUTTON\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str());
        break;
      case OpenZWave::ValueID::ValueType_Raw:
        // No support yet, does not seem necessary
        printf("%i(0x%x:%i): %s: RAW\n", v.GetNodeId(), v.GetCommandClassId(), v.GetIndex(), label.c_str());
        break;
    }
  }
}
