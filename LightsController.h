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
    ~LightsController();

    void SetProgramme( LightsState state );

    private:
    MyZWave::MyNode *uplight;
    MyZWave::MyNode *coffeeTable;
    MyZWave::MyNode *diningTable;
    MyZWave::MyNode *kitchen;
  };
}
#endif
