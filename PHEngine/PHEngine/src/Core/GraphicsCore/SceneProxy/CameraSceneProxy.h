#pragma once

#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <memory>

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

      std::unique_ptr<CameraFrustum> mCameraFrustum;

      glm::vec3 mEyeVector;

      glm::vec3 mForwardVector;
      glm::vec3 mRightVector;
      glm::vec3 mUpVector;

      glm::mat4 mViewMatrix;

      glm::mat4 mProjectionMatrix;

      bool mIsInitializedFirstTime;

   public:

      CameraSceneProxy(const class ACamera* camera);

      glm::vec3 GetEyeVector() const;

      glm::mat4 GetViewMatrix() const;

      glm::mat4 GetProjectionMatrix() const;

      bool IsCameraFrustumBuilt() const;

      bool IsInitializedFirstTime() const;

      CameraFrustum GetCameraFrustum() const;

      ViewPortInfo GetViewPort() const;

      void UpdateViewMatrix(const glm::mat4& viewMatrix);

      void UpdateEyeVector(const glm::vec3& eyeVector);

      void UpdateProjectionMatrix(const glm::mat4& projectionMatrix);

      virtual eCameraSceneProxyType GetCameraSceneType() const;

      void SetForwardVector(const glm::vec3& forwardVector);
      void SetRightVector(const glm::vec3& rightVector);
      void SetUpVector(const glm::vec3& upVector);

      glm::vec3 GetForwardVector() const;
      glm::vec3 GetRightVector() const;
      glm::vec3 GetUpVector() const;

   private:

      void RebuildCameraFrustum();

   };

}
