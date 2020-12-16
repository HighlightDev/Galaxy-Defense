#include "Transform.h"

#include <utility>

namespace Game
{
   TranslationTransform::TranslationTransform(const glm::vec3& translation)
      : Translation(translation)
   {
   }

   RotatorTransform::RotatorTransform(const glm::quat& rotator)
      : Rotator(rotator)
   {
   }

   ScaleTransform::ScaleTransform(const glm::vec3& scale)
      : Scale(scale)
   {
   }

   BoundingBoxTransform::BoundingBoxTransform()
      : TranslationTransform(glm::vec3())
      , ScaleTransform(glm::vec3(1))
   {
   }

   BoundingBoxTransform::BoundingBoxTransform(const glm::vec3& translation, const glm::vec3& scale)
      : TranslationTransform(translation)
      , ScaleTransform(scale)
   {
   }

   Transform::Transform()
      : TranslationTransform(glm::vec3())
      , RotatorTransform(glm::quat())
      , ScaleTransform(glm::vec3(1))
   {
   }

   Transform::Transform(const Transform& transform)
      : TranslationTransform(transform.Translation)
      , RotatorTransform(transform.Rotator)
      , ScaleTransform(transform.Scale)
   {
   }

   Transform::Transform(Transform&& transform)
      : TranslationTransform(std::move(transform.Translation))
      , RotatorTransform(std::move(transform.Rotator))
      , ScaleTransform(std::move(transform.Scale))
   {
   }

   Transform::Transform(const glm::vec3& translation, const glm::quat& rotator, const glm::vec3& scale)
      : TranslationTransform(translation)
      , RotatorTransform(rotator)
      , ScaleTransform(scale)
   {
   }
}