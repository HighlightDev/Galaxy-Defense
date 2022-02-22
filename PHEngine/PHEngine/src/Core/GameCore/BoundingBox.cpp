#include "BoundingBox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace EngineCore
{

   BoundingBox::BoundingBox()
      : mOrigin()
      , mHalfExtent()
   {
   }

   BoundingBox::BoundingBox(const glm::vec3& origin, const glm::vec3& halfExtent)
      : mOrigin(origin)
      , mHalfExtent(halfExtent)
   {
   }

   BoundingBox::~BoundingBox()
   {
   }

   glm::vec3 BoundingBox::GetOrigin() const
   {
      return mOrigin;
   }

   glm::vec3 BoundingBox::GetHalfExtent() const
   {
      return mHalfExtent;
   }

   glm::vec3 BoundingBox::GetMax() const
   {
      return mOrigin + mHalfExtent;
   }

   glm::vec3 BoundingBox::GetMin() const
   {
      return mOrigin - mHalfExtent;
   }

   bool BoundingBox::IsIntersectionWithBox(const BoundingBox& boundingBox) const
   {
      const glm::vec3& meMin = GetMin();
      const glm::vec3& meMax = GetMax();

      const glm::vec3& bbMin = boundingBox.GetMin();
      const glm::vec3& bbMax = boundingBox.GetMax();

      return (meMin.x <= bbMax.x && meMax.x >= bbMin.x) &&
         (meMin.y <= bbMax.y && meMax.y >= bbMin.y) &&
         (meMin.z <= bbMax.z && meMax.z >= bbMin.z);
   }

   std::array<glm::vec3, 8> BoundingBox::GetBoundPositions() const
   {
      /* the sequence is next:
      1 (-,+,-)
      2 (-,-,-)
      3 (+,-,-)
      4 (+,+,-)
      5 (-,+,+)
      6 (-,-,+)
      7 (+,-,+)
      8 (+,+,+)
      */
      return {
        glm::vec3(mOrigin.x - mHalfExtent.x, mOrigin.y + mHalfExtent.y, mOrigin.z - mHalfExtent.z),
        mOrigin - mHalfExtent,
        glm::vec3(mOrigin.x + mHalfExtent.x, mOrigin.y - mHalfExtent.y, mOrigin.z - mHalfExtent.z),
        glm::vec3(mOrigin.x + mHalfExtent.x, mOrigin.y + mHalfExtent.y, mOrigin.z - mHalfExtent.z),
        glm::vec3(mOrigin.x - mHalfExtent.x, mOrigin.y + mHalfExtent.y, mOrigin.z + mHalfExtent.z),
        glm::vec3(mOrigin.x - mHalfExtent.x, mOrigin.y - mHalfExtent.y, mOrigin.z + mHalfExtent.z),
        glm::vec3(mOrigin.x + mHalfExtent.x, mOrigin.y - mHalfExtent.y, mOrigin.z + mHalfExtent.z),
        mOrigin + mHalfExtent,
      };
   }
}
