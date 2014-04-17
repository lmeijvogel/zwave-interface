#ifndef LightsController_H_
#define LightsController_H_
#include "LightsState.h"
#include <stdlib.h>
#include "Common.h"
#include "MyNode.h"

namespace MyZWave {
  class LightsController {
    public:
    LightsController();
    void SetProgramme( LightsState state );

    private:
    uint8 uplightId;
    uint8 coffeeTableId;
    uint8 diningTableId;
    uint8 kitchenId;
  };
}
#endif
