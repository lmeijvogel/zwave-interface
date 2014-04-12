#ifndef Common_H_
#define Common_H_

#include "Defs.h"

// Make the home id global because we're probably always working in one home (I am...)
namespace MyZWave {
  extern uint32 g_homeId;
}
#endif
