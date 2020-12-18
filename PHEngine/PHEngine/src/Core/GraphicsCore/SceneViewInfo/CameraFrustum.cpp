#include "CameraFrustum.h"
#include "Core/UtilityCore/EngineMath.h"
#include <iostream>

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
      const glm::mat4& comboMatrix = projectionMatrix * viewMatrix;

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
         mPlanes[i] = glm::normalize(mPlanes[i]);
      }
   }

   bool CameraFrustum::IsIntersectionWithPointVec3(const glm::vec3& point) const
   {
      return true;
   }

   bool CameraFrustum::IsIntersectionWithBox(const glm::vec3& origin, const glm::vec3& extent) const
   {
      for (const auto& plane : mPlanes)
      {
         float distanceOriginToPlane, absExtentProjectedOnPlaneNormal;

         EngineMath::TestAABBPlane(origin, extent, plane, distanceOriginToPlane, absExtentProjectedOnPlaneNormal);

         const bool bBoundingBoxCollidesWithFrustum =
            (distanceOriginToPlane >= 0.0f || // origin lays on plane or inside this camera frustum plane
               absExtentProjectedOnPlaneNormal >= glm::abs(distanceOriginToPlane)); // check if aabb intersects with plane

         if (!bBoundingBoxCollidesWithFrustum)
            return false;
      }

      return true;
   }

   bool CameraFrustum::CollidesWithBoundingBox(const BoundingBox& boundingBox) const
   {
      return (IsIntersectionWithBox(boundingBox.GetOrigin(), boundingBox.GetHalfExtent()));
   }
}