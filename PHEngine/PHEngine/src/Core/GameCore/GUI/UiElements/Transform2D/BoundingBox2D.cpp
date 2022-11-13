#include "BoundingBox2D.h"

namespace EngineCore
{
   namespace GUI
   {

      BoundingBox2D::BoundingBox2D()
          : mOrigin(), mHalfExtent()
      {
      }

      BoundingBox2D::BoundingBox2D(const glm::vec2 &origin, const glm::vec2 &halfExtent)
          : mOrigin(origin), mHalfExtent(halfExtent)
      {
      }

      BoundingBox2D::~BoundingBox2D()
      {
      }

      glm::vec2 BoundingBox2D::GetOrigin() const
      {
         return mOrigin;
      }

      glm::vec2 BoundingBox2D::GetHalfExtent() const
      {
         return mHalfExtent;
      }

      glm::vec2 BoundingBox2D::GetMax() const
      {
         return mOrigin + mHalfExtent;
      }

      glm::vec2 BoundingBox2D::GetMin() const
      {
         return mOrigin - mHalfExtent;
      }
   }
}
