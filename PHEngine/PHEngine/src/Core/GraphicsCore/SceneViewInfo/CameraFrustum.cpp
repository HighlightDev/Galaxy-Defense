#include "CameraFrustum.h"

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
         mPlanes[i] = glm::normalize(mPlanes[i]);
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
      return true;
   }
}