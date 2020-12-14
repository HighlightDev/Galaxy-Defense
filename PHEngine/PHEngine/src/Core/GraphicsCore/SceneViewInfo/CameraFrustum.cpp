#include "CameraFrustum.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Graphics
{

   CameraFrustum::CameraFrustum()
   {
   }

   CameraFrustum::~CameraFrustum()
   {
   }

   void CameraFrustum::ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) 
   {
      const glm::mat4& comboMatrix = projectionMatrix * glm::transpose(viewMatrix);

      // left clipping plane
      for (int i = 4; i--; ) (mPlanes[0])[i] = comboMatrix[i][3] + comboMatrix[i][0];
      // right clipping plane
      for (int i = 4; i--; ) (mPlanes[1])[i] = comboMatrix[i][3] - comboMatrix[i][0];
      // bottom clipping plane
      for (int i = 4; i--; ) (mPlanes[2])[i] = comboMatrix[i][3] + comboMatrix[i][1];
      // top clipping plane
      for (int i = 4; i--; ) (mPlanes[3])[i] = comboMatrix[i][3] - comboMatrix[i][1];
      // near clipping plane
      for (int i = 4; i--; ) (mPlanes[4])[i] = comboMatrix[i][3] + comboMatrix[i][2];
      // far clipping plane
      for (int i = 4; i--; ) (mPlanes[5])[i] = comboMatrix[i][3] - comboMatrix[i][2];

      // normalize
      for (int i = 0; i < 6; ++i)
      {
         const glm::vec3& normal = glm::normalize(glm::vec3(mPlanes[i]));
         mPlanes[i] = glm::vec4(normal, mPlanes[i].w);
      }
         
   }

   bool CameraFrustum::IsIntersectionWithPointVec3(const glm::vec3& point) const
   {
      return true;
   }

   bool CameraFrustum::IsIntersectionWithSphere(const glm::vec3& origin, const float radius) const
   {
      return true;
   }

   bool CameraFrustum::IsIntersectionWithBoundingBox(const BoundingBox& boundingBox, const bool doSphereTest) const
   {
      const auto& boundPoints = boundingBox.GetBoundPositions();

      for (const auto& plane : mPlanes)
      {
         bool insideFrustum = true;
         for (const auto& point : boundPoints)
         {
            float distance = EngineMath::GetDistancePlaneToPointVec3(point, plane);

            if (distance < 0.0f)
            {
               insideFrustum = false;
               break;
            }
         }

         if (insideFrustum)
            return true;
      }

      return false;
   }
}