#include "AProxyVisibilityController.h"

namespace Graphics
{

   AProxyVisibilityController::AProxyVisibilityController(const bool bVisible)
      : mBoundingBox()
      , mIsVisible(bVisible)
   {
   }

   BoundingBox3D AProxyVisibilityController::GetTransformedBoundingBox() const
   {
      return mBoundingBox;
   }

   void AProxyVisibilityController::SetTransformedBoundingBox(const BoundingBox3D& boundingBox) {

      mBoundingBox = boundingBox;
   }

   bool AProxyVisibilityController::IsVisible() const
   {
      return mIsVisible;
   }

   void AProxyVisibilityController::SetVisibility(const bool visibility)
   {
      mIsVisible = visibility;
   }

}