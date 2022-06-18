#pragma once

namespace EngineCore
{
   struct ComponentData
   {
      std::string GameObjectName;

      ComponentData(const std::string &gameObjectName)
          : GameObjectName(gameObjectName)
      {
      }

      virtual ~ComponentData()
      {
      }
   };

}