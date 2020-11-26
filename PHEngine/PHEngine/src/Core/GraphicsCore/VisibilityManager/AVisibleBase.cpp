#include "AVisibleBase.h"

namespace Graphics
{

   AVisibleBase::AVisibleBase()
      : mIsVisible(true)
      , mBoundingBox()
   {

   }

   bool AVisibleBase::IsVisible() const
   {
      return mIsVisible;
   }

   void AVisibleBase::SetVisibility(const bool visibility)
   {
      mIsVisible = visibility;
   }

   const BoundingBox& AVisibleBase::GetBoundingBox() const
   {
      return mBoundingBox;
   }
}