#include "CameraSceneProxy.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore;
using namespace EngineMath;

namespace Graphics
{
   CameraSceneProxy::CameraSceneProxy(const ACamera *camera)
       : SceneProxyBase(true),
         mViewPort(camera->GetViewPort()),
         mCameraFrustum(),
         mEyeVector(),
         mViewMatrix()
   {
      const auto &perspectiveInfo = camera->GetViewPerspectiveInfo();
      mProjectionMatrix = glm::perspective<float>(perspectiveInfo.FoV, perspectiveInfo.AspectRatio, perspectiveInfo.NearPlane, perspectiveInfo.FarPlane);
   }

   void CameraSceneProxy::UpdateViewMatrix(const glm::mat4 &viewMatrix)
   {
      mViewMatrix = viewMatrix;
      RebuildCameraFrustum();
   }

   void CameraSceneProxy::UpdateEyeVector(const glm::vec3 &eyeVector)
   {
      mEyeVector = eyeVector;
   }

   void CameraSceneProxy::UpdateProjectionMatrix(const glm::mat4 &projectionMatrix)
   {
      mProjectionMatrix = projectionMatrix;
      RebuildCameraFrustum();
   }

   glm::mat4 CameraSceneProxy::GetViewMatrix() const
   {
      return mViewMatrix;
   }

   glm::vec3 CameraSceneProxy::GetEyeVector() const
   {
      return mEyeVector;
   }

   glm::mat4 CameraSceneProxy::GetProjectionMatrix() const
   {
      return mProjectionMatrix;
   }

   bool CameraSceneProxy::IsCameraFrustumBuilt() const
   {
      return mCameraFrustum != nullptr;
   }

   CameraFrustum CameraSceneProxy::GetCameraFrustum() const
   {
      assert(mCameraFrustum);
      return *mCameraFrustum;
   }

   void CameraSceneProxy::RebuildCameraFrustum()
   {
      if (!mCameraFrustum)
      {
         mCameraFrustum = std::make_unique<CameraFrustum>();
      }

      mCameraFrustum->ConstructFromViewProjectionMatrix(mViewMatrix, mProjectionMatrix);
   }

   ViewPortInfo CameraSceneProxy::GetViewPort() const
   {
      return mViewPort;
   }

   eCameraSceneProxyType CameraSceneProxy::GetCameraSceneType() const
   {
      return eCameraSceneProxyType::SECONDARY_SCENE_CAMERA;
   }
}