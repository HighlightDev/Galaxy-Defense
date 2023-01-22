#pragma once

#include <string>

namespace EngineCore
{
   struct ComponentData
   {
      std::string EngineObjectName;

      ComponentData(const std::string &gameObjectName)
          : EngineObjectName(gameObjectName)
      {
      }

      virtual ~ComponentData()
      {
      }
   };

}