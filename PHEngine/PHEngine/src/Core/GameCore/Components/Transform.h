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

   struct EulerRotationTransform
   {
      glm::vec3 RotationEulerAngles;

      EulerRotationTransform(const glm::vec3& eulerRotationAngles);
   };

   struct NoScaleEulerRotationTransform
      : public TranslationTransform
      , public EulerRotationTransform
   {
      NoScaleEulerRotationTransform(const glm::vec3& translation, const glm::vec3& eulerAngles);
   };

   struct BoundingBoxTransform
      : public TranslationTransform
      , public ScaleTransform
   {
      BoundingBoxTransform();

      BoundingBoxTransform(const glm::vec3& translation, const glm::vec3& scale);
   };

   struct EulerAnglesTransform
      : public TranslationTransform
      , public EulerRotationTransform
      , public ScaleTransform
   {

      EulerAnglesTransform();

      EulerAnglesTransform(const EulerAnglesTransform& transform);

      EulerAnglesTransform(const glm::vec3& translation, const glm::vec3& eulerAngles, const glm::vec3& scale);

      EulerAnglesTransform& operator=(const EulerAnglesTransform& t);
   };

   struct Transform 
      : public TranslationTransform
      , public RotatorTransform
      , public ScaleTransform
   {
      Transform();

      Transform(const Transform& transform);

      Transform(const glm::vec3& translation, const glm::quat& rotator, const glm::vec3& scale);

      Transform& operator=(const Transform& t);
   };

}
