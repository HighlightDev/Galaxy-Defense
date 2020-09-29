#pragma once
#include <stdint.h>
#include <unordered_map>
#include <string>

#include "GameObjectProperty.h"

namespace Game
{

   class GameObject 
   {
      static uint64_t mTotalObjectIdCounter;

   protected:

      uint64_t mObjectId;

      std::unordered_map<std::string, GameObjectProperty*> mEngineProperties;

   public:

      const std::string GameObjectName;

   public:

      GameObject(const std::string& gameObjectName);

      uint64_t GetObjectId() const;

      GameObjectProperty* GetEnginePropertyByName(const std::string& key) const;

   };

}
