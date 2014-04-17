#include "LightsController.h"
#include <stdlib.h>

namespace MyZWave {
  LightsController::LightsController() {
    uplightId     = 2;
    coffeeTableId = 4;
    diningTableId = 5;
    kitchenId     = 6;
  }

  void LightsController::SetProgramme( LightsState state ) {
    uint8 uplightIntensity, coffeeTableIntensity, diningTableIntensity, kitchenIntensity;

    switch (state) {
      case Lights_Off:     uplightIntensity     =  0;
                           coffeeTableIntensity =  0;
                           diningTableIntensity =  0;
                           kitchenIntensity     =  0;
      break;
      case Lights_Morning: uplightIntensity     =  0;
                           coffeeTableIntensity = 99;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
      case Lights_Regular: uplightIntensity     = 99;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
      case Lights_Dimmed:  uplightIntensity     = 40;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 40;
                           kitchenIntensity     = 40;
      break;
      case Lights_Night:   uplightIntensity     = 20;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 20;
                           kitchenIntensity     = 20;
      break;
      default:             uplightIntensity     = 99;
                           coffeeTableIntensity = 99;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
    }

    NodeInfo *uplight     = MyZWave::MyNode::FindNodeById(uplightId);
    NodeInfo *coffeeTable = MyZWave::MyNode::FindNodeById(coffeeTableId);
    NodeInfo *diningTable = MyZWave::MyNode::FindNodeById(diningTableId);
    NodeInfo *kitchen     = MyZWave::MyNode::FindNodeById(kitchenId);

    MyZWave::MyNode::SetValue(uplight,     0x26, 0, uplightIntensity);
    MyZWave::MyNode::SetValue(coffeeTable, 0x26, 0, coffeeTableIntensity);
    MyZWave::MyNode::SetValue(diningTable, 0x26, 0, diningTableIntensity);
    MyZWave::MyNode::SetValue(kitchen,     0x26, 0, kitchenIntensity);
  }
}
