#include "CameraFrustum.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Graphics
{

   CameraFrustum::CameraFrustum()
      : viewProjectionMatrix(1)
   {
   }

   CameraFrustum::~CameraFrustum()
   {
   }

   void CameraFrustum::ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) 
   {
      viewProjectionMatrix = projectionMatrix * viewMatrix;
      const glm::mat4& comboMatrix = projectionMatrix * viewMatrix;

      //// Left
      //mPlanes[0].x = comboMatrix[3][0] + comboMatrix[0][0];
      //mPlanes[0].y = comboMatrix[3][1] + comboMatrix[0][1];
      //mPlanes[0].z = comboMatrix[3][2] + comboMatrix[0][2];
      //mPlanes[0].w = comboMatrix[3][3] + comboMatrix[0][3];
      //mPlanes[0] = glm::normalize(mPlanes[0]);

      //// Right
      //mPlanes[1].x = comboMatrix[3][0] - comboMatrix[0][0];
      //mPlanes[1].y = comboMatrix[3][1] - comboMatrix[0][1];
      //mPlanes[1].z = comboMatrix[3][2] - comboMatrix[0][2];
      //mPlanes[1].w = comboMatrix[3][3] - comboMatrix[0][3];
      //mPlanes[1] = glm::normalize(mPlanes[1]);

      //// Top
      //mPlanes[2].x = comboMatrix[3][0] - comboMatrix[1][0];
      //mPlanes[2].y = comboMatrix[3][1] - comboMatrix[1][1];
      //mPlanes[2].z = comboMatrix[3][2] - comboMatrix[1][2];
      //mPlanes[2].w = comboMatrix[3][3] - comboMatrix[1][3];
      //mPlanes[2] = glm::normalize(mPlanes[2]);

      //// Bottom
      //mPlanes[3].x = comboMatrix[3][0] + comboMatrix[1][0];
      //mPlanes[3].y = comboMatrix[3][1] + comboMatrix[1][1];
      //mPlanes[3].z = comboMatrix[3][2] + comboMatrix[1][2];
      //mPlanes[3].w = comboMatrix[3][3] + comboMatrix[1][3];
      //mPlanes[3] = glm::normalize(mPlanes[3]);

      //// Near
      //mPlanes[4].x = comboMatrix[2][0];
      //mPlanes[4].y = comboMatrix[2][1];
      //mPlanes[4].z = comboMatrix[2][2];
      //mPlanes[4].w = comboMatrix[2][3];
      //mPlanes[4] = glm::normalize(mPlanes[4]);

      //// Far
      //mPlanes[5].x = comboMatrix[3][0] - comboMatrix[2][0];
      //mPlanes[5].y = comboMatrix[3][1] - comboMatrix[2][1];
      //mPlanes[5].z = comboMatrix[3][2] - comboMatrix[2][2];
      //mPlanes[5].w = comboMatrix[3][3] - comboMatrix[2][3];
      //mPlanes[5] = glm::normalize(mPlanes[5]);

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



      //// Right clipping plane.
      //mPlanes[0].set(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8], mvp[15] - mvp[12]);
      //// Left clipping plane.
      //mPlanes[1].set(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8], mvp[15] + mvp[12]);
      //// Bottom clipping plane.
      //mPlanes[2].set(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9], mvp[15] + mvp[13]);
      //// Top clipping plane.
      //mPlanes[3].set(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9], mvp[15] - mvp[13]);
      //// Far clipping plane.
      //mPlanes[4].set(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]);
      //// Near clipping plane.
      //mPlanes[5].set(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]);

      //// Normalize, this is not always necessary...
      //for (unsigned int i = 0; i < 6; i++)
      //{
      //   mPlanes[i].normalize();
      //}

         
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

      //bool insideFrustum = false;
      //for (const auto& point : boundPoints)
      //{
      //   glm::vec4 clippedSpace = viewProjectionMatrix * glm::vec4(point, 1.0f);
      //   glm::vec3 ndc = glm::vec3(clippedSpace) / clippedSpace.w;
      //   if ((ndc.x > -1.0f && ndc.x < 1.0f) && 
      //      (ndc.y > -1.0f && ndc.y < 1.0f) &&
      //      (ndc.z > -1.0f && ndc.z < 1.0f))
      //   {
      //      insideFrustum = true;
      //      break;
      //   }
      //}

      //if (insideFrustum)
      //   return true;

      for (const auto& point : boundPoints)
      {
         bool insideFrustum = true;
         for (const auto& plane : mPlanes)
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