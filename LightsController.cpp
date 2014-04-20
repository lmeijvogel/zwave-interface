#include "LightsController.h"
#include <stdlib.h>

namespace MyZWave {
  LightsController::LightsController() {
    uint8 uplightId     = 2;
    uint8 coffeeTableId = 4;
    uint8 diningTableId = 5;
    uint8 kitchenId     = 7;

    NodeInfo *uplightNi     = MyZWave::MyNode::FindNodeById(uplightId);
    NodeInfo *coffeeTableNi = MyZWave::MyNode::FindNodeById(coffeeTableId);
    NodeInfo *diningTableNi = MyZWave::MyNode::FindNodeById(diningTableId);
    NodeInfo *kitchenNi     = MyZWave::MyNode::FindNodeById(kitchenId);

    uplight     = new MyZWave::MyNode(uplightNi);
    coffeeTable = new MyZWave::MyNode(coffeeTableNi);
    diningTable = new MyZWave::MyNode(diningTableNi);
    kitchen     = new MyZWave::MyNode(kitchenNi);
  }

  LightsController::~LightsController() {
    delete uplight;
    delete coffeeTable;
    delete diningTable;
    delete kitchen;
  }

  void LightsController::SetProgramme( LightsState state ) {
    uint8 uplightIntensity, coffeeTableIntensity, diningTableIntensity;
    bool kitchenIntensity;

    switch (state) {
      case Lights_Off:     uplightIntensity     =  0;
                           coffeeTableIntensity =  0;
                           diningTableIntensity =  0;
                           kitchenIntensity     =  false;
      break;
      case Lights_Morning: uplightIntensity     =  0;
                           coffeeTableIntensity = 99;
                           diningTableIntensity = 99;
                           kitchenIntensity     = true ;
      break;
      case Lights_Regular: uplightIntensity     = 99;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 99;
                           kitchenIntensity     = true;
      break;
      case Lights_Dimmed:  uplightIntensity     = 40;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 40;
                           kitchenIntensity     = true;
      break;
      case Lights_Night:   uplightIntensity     = 20;
                           coffeeTableIntensity =  0;
                           diningTableIntensity = 20;
                           kitchenIntensity     = 20;
      break;
      default:             uplightIntensity     = 99;
                           coffeeTableIntensity = 99;
                           diningTableIntensity = 99;
                           kitchenIntensity     = true;
      break;
    }

    uplight->SetValue(0x26, 0, uplightIntensity);
    coffeeTable->SetValue(0x26, 0, coffeeTableIntensity);
    diningTable->SetValue(0x26, 0, diningTableIntensity);
    kitchen->SetValue(0x25, 0, kitchenIntensity);
  }
}
