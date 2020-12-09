#pragma once

#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"

namespace Game
{
   class ACamera;
}

namespace Graphics
{
   using Game::ACamera;

   class CameraSceneProxy
      : public SceneProxyBase
   {
      CameraFrustum mCameraFrustum;

      glm::vec3 mEyeVector;

      glm::mat4 mViewMatrix;

   public:

      CameraSceneProxy(const class ACamera* camera);

      glm::vec3 GetEyeVector() const;

      glm::mat4 GetViewMatrix() const;

      void UpdateViewMatrix(const glm::mat4& viewMatrix);

      void UpdateEyeVector(const glm::vec3& eyeVector);

   };

}
