#include "Transform.h"

#include <utility>

namespace Game
{
   Transform::Transform()
      : Translation()
      , Rotator()
      , Scale(1)
   {
   }

   Transform::Transform(const Transform& transform)
   {
      this->Translation = transform.Translation;
      this->Rotator = transform.Rotator;
      this->Scale = transform.Scale;
   }

   Transform::Transform(Transform&& transform)
   {
      if (this != &transform)
      {
         this->Translation = std::move(transform.Translation);
         this->Rotator = std::move(transform.Rotator);
         this->Scale = std::move(transform.Scale);
      }
   }

   Transform::Transform(const glm::vec3& translation, const glm::quat& rotator, const glm::vec3& scale)
   {
      this->Translation = translation;
      this->Rotator = rotator;
      this->Scale = scale;
   }
}