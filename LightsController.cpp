#include "LightsController.h"
#include <stdlib.h>

namespace MyZWave {
  LightsController::LightsController() {
    uint8 uplightId     = 2;
    uint8 coffeeTableId = 7;
    uint8 diningTableId = 5;
    uint8 kitchenId     = 8;

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

  void LightsController::SetProgramme( Programme programme ) {
    uint8 uplightIntensity, diningTableIntensity, kitchenIntensity;
    bool coffeeTableOn;

    switch (programme) {
      case Lights_Off:     uplightIntensity     =  0;
                           coffeeTableOn        = false;
                           diningTableIntensity =  0;
                           kitchenIntensity     =  0;
      break;
      case Lights_Morning: uplightIntensity     =  0;
                           coffeeTableOn        = true;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
      case Lights_Regular: uplightIntensity     = 99;
                           coffeeTableOn        = false;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
      case Lights_Dimmed:  uplightIntensity     = 40;
                           coffeeTableOn        = false;
                           diningTableIntensity = 50;
                           kitchenIntensity     = 40;
      break;
      case Lights_Night:   uplightIntensity     = 25;
                           coffeeTableOn        = false;
                           diningTableIntensity =  0;
                           kitchenIntensity     = 25;
      break;
      default:             uplightIntensity     = 99;
                           coffeeTableOn        = true;
                           diningTableIntensity = 99;
                           kitchenIntensity     = 99;
      break;
    }

    uplight->SetValue(0x26, 0, uplightIntensity);
    coffeeTable->SetValue(0x25, 0, coffeeTableOn       );
    diningTable->SetValue(0x26, 0, diningTableIntensity);
    kitchen->SetValue(0x26, 0, kitchenIntensity);
  }
}
