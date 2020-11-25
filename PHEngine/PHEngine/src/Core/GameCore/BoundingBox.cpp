#include "BoundingBox.h"

namespace Game
{

   BoundingBox::BoundingBox(const glm::vec3& origin, const glm::vec3& extent)
      : mOrigin(origin)
      , mExtent(extent)
   {
   }

   BoundingBox::~BoundingBox()
   {
   }

   void BoundingBox::UpdateScale(const glm::vec3& scale)
   {
      mScale = scale;
   }

   void BoundingBox::UpdateTranslation(const glm::vec3& translation)
   {
      mTranslation = translation;
   }
}
