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
      static uint64_t mTotalObjectIdCounter;

   protected:

      uint64_t mObjectId;

      std::unordered_map<std::string, std::shared_ptr<EngineGOPropertyBase>> mEngineProperties;

      std::string EngineObjectName;

   public:

      EngineObject(const std::string& gameObjectName);

      uint64_t GetObjectId() const;

      const std::shared_ptr<EngineGOPropertyBase>& GetEnginePropertyByName(const std::string& key) const;

      void AddEngineProperty(const std::shared_ptr<EngineGOPropertyBase>& goPtr);

      std::string GetEngineObjectName() const;

   };

}
