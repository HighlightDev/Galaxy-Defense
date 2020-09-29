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

   GameObjectProperty* GameObject::GetEnginePropertyByName(const std::string& key) const
   {
      assert((mEngineProperties.count(key)));
      return mEngineProperties.at(key);
   }
}
