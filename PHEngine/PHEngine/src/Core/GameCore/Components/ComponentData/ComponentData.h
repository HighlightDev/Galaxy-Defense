#pragma once

#include "Core/GameCore/Components/ComponentType.h"

namespace Game
{
   struct ComponentData {
      std::string GameObjectName;

      ComponentData(const std::string& gameObjectName)
         : GameObjectName(gameObjectName)
      {

      }
   };

}