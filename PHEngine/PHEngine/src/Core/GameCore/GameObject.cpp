#pragma once
#include <stdint.h>

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

   void GameObject::AddEngineProperty(const std::string& key, GameObjectProperty* goPtr) {
      assert((!mEngineProperties.count(key)));
      mEngineProperties[key] = goPtr;
   }

   GameObjectProperty* GameObject::GetEnginePropertyByName(const std::string& key) const
   {
      assert((mEngineProperties.count(key)));
      return mEngineProperties.at(key);
   }

   std::string GameObject::GetGameObjectName() const
   {
      return GameObjectName;
   }
}
