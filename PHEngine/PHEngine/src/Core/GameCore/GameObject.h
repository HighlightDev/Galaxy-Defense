#pragma once
#include <stdint.h>
#include <unordered_map>
#include <string>

#include "GameObjectProperty.h"

namespace Game
{
#define ENGINE_PROPERTY(NAME, PROPERTY_PTR) AddEngineProperty(NAME, PROPERTY_PTR)
   class GameObject 
   {
      static uint64_t mTotalObjectIdCounter;

   protected:

      uint64_t mObjectId;

      std::unordered_map<std::string, EngineGOPropertyBase*> mEngineProperties;

      std::string GameObjectName;

   public:

      GameObject(const std::string& gameObjectName);

      uint64_t GetObjectId() const;

      EngineGOPropertyBase* GetEnginePropertyByName(const std::string& key) const;

      void AddEngineProperty(const std::string& key, EngineGOPropertyBase* goPtr);

      std::string GetGameObjectName() const;

   };

}
