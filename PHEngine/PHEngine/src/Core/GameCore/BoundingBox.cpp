#include "BoundingBox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Game
{

   BoundingBox::BoundingBox()
      : mOrigin()
      , mHalfExtent()
      , mRadius(0)
   {
   }

   BoundingBox::BoundingBox(const glm::vec3& origin, const glm::vec3& halfExtent)
      : mOrigin(origin)
      , mHalfExtent(halfExtent)
      , mRadius(glm::length(halfExtent))
   {
   }

   BoundingBox::~BoundingBox()
   {
   }

   glm::vec3 BoundingBox::GetTransformedOrigin(const glm::vec3& translation) const
   {
      return mOrigin + translation;
   }

   glm::vec3 BoundingBox::GetTransformedExtent(const glm::vec3& scale) const
   {
      return mHalfExtent * scale;
   }

   glm::vec3 BoundingBox::GetOrigin() const
   {
      return mOrigin;
   }

   glm::vec3 BoundingBox::GetHalfExtent() const
   {
      return mHalfExtent;
   }

   float BoundingBox::GetRadius() const 
   {
      return mRadius;
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

   BoundingBox BoundingBox::GetMeTransformed(const glm::vec3& translation, const glm::vec3& scale) const
   {
      return BoundingBox(mOrigin + translation, mHalfExtent * scale);
   }

   BoundingBox BoundingBox::GetMeTransformed(const glm::mat4& transformMatrix) const
   {
      std::array<glm::vec3, 8> bbPoints = GetBoundPositions();

      glm::vec4 startPoint = transformMatrix * glm::vec4(*bbPoints.begin(), 1.0f);
      glm::vec3 maxPoint = startPoint;
      glm::vec3 minPoint = maxPoint;
      
      for (auto pointIt = std::next(bbPoints.begin(), 1); pointIt != bbPoints.end(); ++pointIt)
      {
         const glm::vec4& result = transformMatrix * glm::vec4(*pointIt, 1.0f);

         maxPoint.x = glm::max(result.x, maxPoint.x);
         maxPoint.y = glm::max(result.y, maxPoint.y);
         maxPoint.z = glm::max(result.z, maxPoint.z);

         minPoint.x = glm::min(result.x, minPoint.x);
         minPoint.y = glm::min(result.y, minPoint.y);
         minPoint.z = glm::min(result.z, minPoint.z);
      }

      const glm::vec3& halfExtent = glm::abs(maxPoint - minPoint) / 2.0f;
      const glm::vec3& origin = minPoint + halfExtent;

      return BoundingBox(origin, halfExtent);
   }
}
