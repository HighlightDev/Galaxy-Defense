#pragma once

#include <string>
#include <stdint.h>
#include "Core/CommonCore/Assertion.h"

namespace Game
{
   enum ComponentType
   {
      COMPONENT = 1,
      SCENE_COMPONENT = 2,
      PHYSICS_COMPONENT = 4,
      LIGHT_COMPONENT = SCENE_COMPONENT | 8,
      PRIMITIVE_COMPONENT = SCENE_COMPONENT | 16,
      INPUT_COMPONENT = COMPONENT | 32,
      CHARACTER_MOVEMENT_COMPONENT = COMPONENT | 64,
      MOVEMENT_COMPONENT = COMPONENT | 128,
      PLANAR_REFLECTION_COMPONENT = SCENE_COMPONENT | 256
   };
}
