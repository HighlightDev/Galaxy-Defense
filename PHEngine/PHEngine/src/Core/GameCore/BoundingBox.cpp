#include "BoundingBox.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Game
{

   BoundingBox::BoundingBox()
      : mOrigin()
      , mExtent()
   {
   }

   BoundingBox::BoundingBox(const glm::vec3& origin, const glm::vec3& extent)
      : mOrigin(origin)
      , mExtent(extent)
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
      return mExtent * scale;
   }

   glm::vec3 BoundingBox::GetOrigin() const
   {
      return mOrigin;
   }

   glm::vec3 BoundingBox::GetExtent() const
   {
      return mExtent;
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
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y + mExtent.y, mOrigin.z - mExtent.z),
        mOrigin - mExtent,
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y - mExtent.y, mOrigin.z - mExtent.z),
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y + mExtent.y, mOrigin.z - mExtent.z),
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y + mExtent.y, mOrigin.z + mExtent.z),
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y - mExtent.y, mOrigin.z + mExtent.z),
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y - mExtent.y, mOrigin.z + mExtent.z),
        mOrigin + mExtent,
      };
   }

   BoundingBox BoundingBox::GetMeTransformed(const glm::mat4& transformMatrix) const
   {
      std::array<glm::vec3, 8> bbPoints = GetBoundPositions();

      glm::vec3 maxPoint = *bbPoints.begin(), minPoint = *bbPoints.begin();

      for (auto pointIt = std::next(bbPoints.begin(), 1); pointIt != bbPoints.end(); ++pointIt)
      {
         const glm::vec3& result = (glm::vec4(*pointIt, 1.0f)) * transformMatrix;

         maxPoint.x = glm::max(result.x, maxPoint.x);
         maxPoint.y = glm::max(result.y, maxPoint.y);
         maxPoint.z = glm::max(result.z, maxPoint.z);

         minPoint.x = glm::min(result.x, minPoint.x);
         minPoint.y = glm::min(result.y, minPoint.y);
         minPoint.z = glm::min(result.z, minPoint.z);
      }

      const glm::vec3& extent = glm::abs(maxPoint - minPoint) / 2.0f;
      const glm::vec3& origin = minPoint + extent;

      return BoundingBox(origin, extent);
   }
}
