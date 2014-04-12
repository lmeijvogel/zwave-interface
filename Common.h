#ifndef Common_H_
#define Common_H_

#include "Defs.h"
#include "Node.h"

// Make the home id global because we're probably always working in one home (I am...)
namespace MyZWave {
  extern uint32 g_homeId;
}

typedef struct
{
  uint32      m_homeId;
  uint8      m_nodeId;
  bool      m_polled;
  std::list<OpenZWave::ValueID>  m_values;
}NodeInfo;
#endif
