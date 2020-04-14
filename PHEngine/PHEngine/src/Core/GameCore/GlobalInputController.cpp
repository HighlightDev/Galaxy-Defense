#include "GlobalInputController.h"

namespace Game
{
   GlobalInputController::GlobalInputController() { }

   GlobalInputController* GlobalInputController::mInstance = nullptr;

   GlobalInputController* GlobalInputController::GetInstance() {

      if (!mInstance)
         mInstance = new GlobalInputController();

      return mInstance;
   }
}
