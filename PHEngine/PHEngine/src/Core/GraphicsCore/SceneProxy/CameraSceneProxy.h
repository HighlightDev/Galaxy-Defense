#pragma once

#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace EngineCore
{
   class ACamera;
}

namespace Graphics
{
   using EngineCore::ACamera;

   enum class eCameraSceneProxyType
   {
      MAIN_SCENE_CAMERA,
      SECONDARY_SCENE_CAMERA
   };

   class CameraSceneProxy
      : public SceneProxyBase
   {
      ViewPortInfo mViewPort;

      CameraFrustum mCameraFrustum;

      glm::vec3 mEyeVector;

      glm::mat4 mViewMatrix;

      glm::mat4 mProjectionMatrix;

   public:

      CameraSceneProxy(const class ACamera* camera);

      glm::vec3 GetEyeVector() const;

      glm::mat4 GetViewMatrix() const;

      glm::mat4 GetProjectionMatrix() const;

      const CameraFrustum& GetCameraFrustum() const;

      ViewPortInfo GetViewPort() const;

      void UpdateViewMatrix(const glm::mat4& viewMatrix);

      void UpdateEyeVector(const glm::vec3& eyeVector);

      void UpdateProjectionMatrix(const glm::mat4& projectionMatrix);

      virtual eCameraSceneProxyType GetCameraSceneType() const;

   private:

      void RebuildCameraFrustum();

   };

}
