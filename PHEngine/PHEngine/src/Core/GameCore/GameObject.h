#pragma once
#include <stdint.h>

namespace Game
{

   class GameObject 
   {
      static uint64_t mTotalObjectIdCounter;

      uint64_t mObjectId;

   public:

      GameObject();

      uint64_t GetObjectId() const;

   };

}
