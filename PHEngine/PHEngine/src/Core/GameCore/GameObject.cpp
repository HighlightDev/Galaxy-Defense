#include <cstddef>

#include "GameObject.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   uint64_t GameObject::mTotalObjectIdCounter = 0;

   GameObject::GameObject(const std::string& gameObjectName)
      : mObjectId(mTotalObjectIdCounter++)
      , GameObjectName(gameObjectName)
   {
   }

   uint64_t GameObject::GetObjectId() const
   {
      return mObjectId;
   }

   void GameObject::AddEngineProperty(EngineGOPropertyBase& goPtr) {
      assert((!mEngineProperties.count(goPtr.Key)));
      mEngineProperties[goPtr.Key] = &goPtr;
   }

   EngineGOPropertyBase* GameObject::GetEnginePropertyByName(const std::string& key) const
   {
      assert((mEngineProperties.count(key)));
      return mEngineProperties.at(key);
   }

   std::string GameObject::GetGameObjectName() const
   {
      return GameObjectName;
   }
}
