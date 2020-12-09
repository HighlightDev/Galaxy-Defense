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

   BoundingBox BoundingBox::GetMeTransformed(const glm::mat4& transformMatrix) const
   {
      glm::vec3 bbPoints[8] =
      { mOrigin + mExtent,
        mOrigin - mExtent,
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y + mExtent.y, mOrigin.z + mExtent.z),
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y + mExtent.y, mOrigin.z - mExtent.z),
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y + mExtent.y, mOrigin.z - mExtent.z),
        glm::vec3(mOrigin.x - mExtent.x, mOrigin.y - mExtent.y, mOrigin.z + mExtent.z),
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y - mExtent.y, mOrigin.z - mExtent.z),
        glm::vec3(mOrigin.x + mExtent.x, mOrigin.y - mExtent.y, mOrigin.z + mExtent.z),
      };

      return BoundingBox();
   }
}
