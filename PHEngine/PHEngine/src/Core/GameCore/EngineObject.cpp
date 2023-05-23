#include <cstddef>

#include "EngineObject.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   uint32_t EngineObject::mTotalObjectIdCounter = 0;

   EngineObject::EngineObject(const std::string& gameObjectName)
      : mObjectId(mTotalObjectIdCounter++)
      , EngineObjectName(gameObjectName)
   {
   }

   uint32_t EngineObject::GetObjectId() const
   {
      return mObjectId;
   }

   void EngineObject::AddEngineProperty(const std::shared_ptr<EngineObjectPropertyBase>& goPtr) {
      assert((!mEngineProperties.count(goPtr->Key)));
      mEngineProperties[goPtr->Key] = goPtr;
   }

   const std::shared_ptr<EngineObjectPropertyBase>& EngineObject::GetEnginePropertyByName(const std::string& key) const
   {
      assert((mEngineProperties.count(key)));
      return mEngineProperties.at(key);
   }

   std::string EngineObject::GetEngineObjectName() const
   {
      return EngineObjectName;
   }
}
