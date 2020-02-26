#pragma once
#include <stdint.h>

#include "GameObject.h"

namespace Game
{

   uint64_t GameObject::mTotalObjectIdCounter = 0;

   GameObject::GameObject()
      : mObjectId(mTotalObjectIdCounter++)
   {
   }

   uint64_t GameObject::GetObjectId() const
   {
      return mObjectId;
   }
}
