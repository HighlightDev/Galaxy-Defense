#pragma once

#include "Keys.h"

namespace EngineCore {

   template <typename KeysType>
   struct InputDeviceKeyData
   {
      KeysType Key;
      KeyState State;

      InputDeviceKeyData(KeysType key, KeyState state)
         : Key(key)
         , State(state)
      {
      }

      InputDeviceKeyData()
         : Key(eKeyboardKeys::None)
         , State(KeyState::RELEASED)
      {
      }
   };

   using KeyboardKeysData = InputDeviceKeyData<eKeyboardKeys>;
   using MouseKeysData = InputDeviceKeyData<eMouseKeys>;
}