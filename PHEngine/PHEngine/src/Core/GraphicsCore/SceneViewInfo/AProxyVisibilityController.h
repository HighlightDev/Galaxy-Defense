#pragma once

#include "Core/GameCore/BoundingBox.h"

using namespace Game;

namespace Graphics
{

   class AProxyVisibilityController
   {
   protected:

      BoundingBox mBoundingBox;

      bool mIsVisible;

   public:

      AProxyVisibilityController(const bool bVisible);

      BoundingBox GetTransformedBoundingBox() const;

      void SetTransformedBoundingBox(const BoundingBox& boundingBox);

      // Method returns false when no frustum call is needed for primitive
      virtual bool IsFrustumCullTestNeeded() const = 0;

      void SetVisibility(const bool visibility);

      bool IsVisible() const;

   };

}
