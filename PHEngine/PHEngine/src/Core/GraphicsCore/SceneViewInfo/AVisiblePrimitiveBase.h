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

      BoundingBox GetTransformedBoundingBox() const;

      void SetTransformedBoundingBox(const BoundingBox& boundingBox);
   };

}
