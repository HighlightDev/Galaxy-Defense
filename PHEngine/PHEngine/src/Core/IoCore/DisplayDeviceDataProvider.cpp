#include "DisplayDeviceDataProvider.h"

namespace IO
{
   DisplayDeviceDataProvider::DisplayDeviceDataProvider() { }

   DisplayDeviceDataProvider* DisplayDeviceDataProvider::GetInstance() {
      static DisplayDeviceDataProvider deviceSettings;
      return &deviceSettings;
   }
}
