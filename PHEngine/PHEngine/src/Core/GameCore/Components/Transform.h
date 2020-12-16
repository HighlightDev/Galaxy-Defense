#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

namespace Game
{

   struct TranslationTransform
   {
      glm::vec3 Translation;

      TranslationTransform(const glm::vec3& translation);
   };

   struct ScaleTransform
   {
      glm::vec3 Scale;

      ScaleTransform(const glm::vec3& scale);
   };

   struct RotatorTransform
   {
      glm::quat Rotator;

      RotatorTransform(const glm::quat& rotator);
   };

   struct BoundingBoxTransform
      : public TranslationTransform
      , public ScaleTransform
   {
      BoundingBoxTransform();

      BoundingBoxTransform(const glm::vec3& translation, const glm::vec3& scale);
   };

   struct Transform 
      : public TranslationTransform
      , public RotatorTransform
      , public ScaleTransform
   {
      Transform();

      Transform(const Transform& transform);

      Transform(Transform&& transform);

      Transform(const glm::vec3& translation, const glm::quat& rotator, const glm::vec3& scale);

      Transform& operator=(const Transform& t)
      {
         Translation = t.Translation;
         Rotator = t.Rotator;
         Scale = t.Scale;
         return *this;
      }
   };

}
