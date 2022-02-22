#include "CameraSceneProxy.h"

#include "Core/GameCore/ACamera.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore;
using namespace EngineMath;

namespace Graphics
{
   CameraSceneProxy::CameraSceneProxy(const ACamera* camera)
      : SceneProxyBase()
      , mViewPort(camera->GetViewPort())
      , mEyeVector()
      , mViewMatrix()
   {
      const auto& perspectiveInfo = camera->GetViewPerspectiveInfo();
      mProjectionMatrix = glm::perspective<float>(perspectiveInfo.FoV, perspectiveInfo.AspectRatio, perspectiveInfo.NearPlane, perspectiveInfo.FarPlane);
   }

   void CameraSceneProxy::UpdateViewMatrix(const glm::mat4& viewMatrix)
   {
      mViewMatrix = viewMatrix;
      RebuildCameraFrustum();
   }

   void CameraSceneProxy::UpdateEyeVector(const glm::vec3& eyeVector)
   {
      mEyeVector = eyeVector;
   }

   void CameraSceneProxy::UpdateProjectionMatrix(const glm::mat4& projectionMatrix) {
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

   glm::mat4 CameraSceneProxy::GetProjectionMatrix() const {
      return mProjectionMatrix;
   }

   const CameraFrustum& CameraSceneProxy::GetCameraFrustum() const
   {
      return mCameraFrustum;
   }

   void CameraSceneProxy::RebuildCameraFrustum()
   {
      mCameraFrustum.ConstructFromViewProjectionMatrix(mViewMatrix, mProjectionMatrix);
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