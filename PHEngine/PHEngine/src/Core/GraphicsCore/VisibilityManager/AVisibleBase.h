#pragma once

#include "Core/GameCore/BoundingBox.h"

using namespace Game;

namespace Graphics
{

   class AVisibleBase
   {
   protected:

      bool mIsVisible;

      BoundingBox mBoundingBox;

   public:

      AVisibleBase();

      void SetVisibility(const bool visibility);

      bool IsVisible() const;

      const BoundingBox& GetBoundingBox() const;

   };

}
