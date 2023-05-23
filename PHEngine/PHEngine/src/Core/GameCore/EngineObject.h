#pragma once
#include <cstddef>
#include <unordered_map>
#include <string>
#include <memory>

#include "EngineObjectProperty.h"

namespace EngineCore
{
   class EngineObject 
   {
      static uint32_t mTotalObjectIdCounter;

   protected:

      uint32_t mObjectId;

      std::unordered_map<std::string, std::shared_ptr<EngineObjectPropertyBase>> mEngineProperties;

      std::string EngineObjectName;

   public:

      EngineObject(const std::string& gameObjectName);

      uint32_t GetObjectId() const;

      const std::shared_ptr<EngineObjectPropertyBase>& GetEnginePropertyByName(const std::string& key) const;

      void AddEngineProperty(const std::shared_ptr<EngineObjectPropertyBase>& goPtr);

      std::string GetEngineObjectName() const;

   };

}
