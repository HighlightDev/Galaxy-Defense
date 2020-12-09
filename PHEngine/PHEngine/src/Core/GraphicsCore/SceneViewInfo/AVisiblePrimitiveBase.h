#pragma once

#include "Core/GameCore/BoundingBox.h"

using namespace Game;

namespace Graphics
{

   class AVisiblePrimitiveBase
   {
   protected:

      BoundingBox mBoundingBox;

   public:

      AVisiblePrimitiveBase();

      const BoundingBox& GetBoundingBox() const;

      virtual const BoundingBox& GetTransformedBoundingBox() const = 0;

   };

}
