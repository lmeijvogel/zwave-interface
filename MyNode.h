#ifndef MyNode_H_
#define MyNode_H_
#include <stdlib.h>

#include "Common.h"
#include "Node.h"

using namespace std;

/* Helper class to group a lot of Node-related methods together.
 * This should eventually become a normal class where methods
 * are called on instances.
 */
namespace MyZWave {
  class MyNode {
    public:

    static NodeInfo *FindNodeById(uint8 id);

    static bool GetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 *value);
    static bool SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 value);
    static bool SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, int value);
    static bool SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, bool value);

    static void PrintNode(NodeInfo *nodeInfo);
    static void PrintValueID(OpenZWave::ValueID v);

    static list<NodeInfo*> nodes;
  };
}
#endif
