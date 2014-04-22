#ifndef LightsController_H_
#define LightsController_H_
#include "Programme.h"
#include <stdlib.h>
#include "Common.h"
#include "MyNode.h"

namespace MyZWave {
  class LightsController {
    public:
    LightsController();
    ~LightsController();

    void SetProgramme( Programme Programme );

    private:
    MyZWave::MyNode *uplight;
    MyZWave::MyNode *coffeeTable;
    MyZWave::MyNode *diningTable;
    MyZWave::MyNode *kitchen;
  };
}
#endif
