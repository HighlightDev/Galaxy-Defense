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

   void BoundingBox::UpdateScale(const glm::vec3& scale)
   {
      mScale = scale;
   }

   void BoundingBox::UpdateTranslation(const glm::vec3& translation)
   {
      mTranslation = translation;
   }

   glm::vec3 BoundingBox::GetOrigin() const
   {
      glm::mat4 worldMatrix(1);

      const glm::mat4& translationM = glm::translate(worldMatrix, mTranslation);
      const glm::mat4& scalingM = glm::scale(worldMatrix, mScale);

      worldMatrix *= scalingM;
      worldMatrix *= translationM;
      
      return worldMatrix * glm::vec4(mOrigin.x, mOrigin.y, mOrigin.z, 1);
   }

   glm::vec3 BoundingBox::GetExtent() const
   {
      return mExtent * mScale;
   }
}
