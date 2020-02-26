#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

namespace Graphics
{
   namespace Animation
   {

      class BoneTransform
      {
         using mat4 = glm::mat4;
         using vec3 = glm::vec3;

         // Transformation is relative to parent bone

         // matrix to bone local space
         glm::mat4 m_toBoneSpace;
         glm::quat m_toBoneRotation;
         glm::vec3 m_toBoneTranslation;
         glm::vec3 m_toBoneScale;

      public:

         BoneTransform(mat4 toBoneMatrix)
         {
            m_toBoneSpace = toBoneMatrix;
            ExtractRotationTranslationScale();
         }

         BoneTransform(glm::quat rotation, vec3 translation, vec3 scale)
         {
            m_toBoneRotation = rotation;
            m_toBoneTranslation = translation;
            m_toBoneScale = scale;

            glm::mat4 identityMatrix(1);
            glm::mat4 translationMatrix = glm::translate(identityMatrix, translation);
            glm::mat4 scaleMatrix = glm::scale(identityMatrix, scale);
            glm::mat4 rotationMatrix = glm::toMat4(rotation);

            m_toBoneSpace = translationMatrix * rotationMatrix * scaleMatrix;
         }

         BoneTransform()
            : m_toBoneSpace(1)
            , m_toBoneRotation()
            , m_toBoneTranslation(0)
            , m_toBoneScale(1)
         {
         }

         static BoneTransform SLerp(BoneTransform lhv, BoneTransform rhv, float blend)
         {
            vec3 lerpPosition = glm::lerp(lhv.m_toBoneTranslation, rhv.m_toBoneTranslation, blend);
            vec3 lerpScale = glm::lerp(lhv.m_toBoneScale, rhv.m_toBoneScale, blend);
            glm::quat lerpRotation = glm::slerp(lhv.m_toBoneRotation, rhv.m_toBoneRotation, blend);
            return BoneTransform(lerpRotation, lerpPosition, lerpScale);
         }

         void SetToBoneSpaceMatrix(mat4 toBoneSpaceMatrix)
         {
            m_toBoneSpace = toBoneSpaceMatrix;
            ExtractRotationTranslationScale();
         }

         mat4 GetToBoneSpaceMatrix() const
         {
            return m_toBoneSpace;
         }

         void ExtractRotationTranslationScale()
         {
            glm::mat4 transformation; // your transformation matrix.
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(transformation, scale, rotation, translation, skew, perspective);

            m_toBoneRotation = rotation;
            m_toBoneTranslation = translation;
            m_toBoneScale = scale;
         }
      };

   }
}