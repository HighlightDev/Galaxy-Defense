#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

namespace Game
{

   struct Transform
   {
      glm::vec3 Translation;
      glm::quat Rotator;
      glm::vec3 Scale;

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
