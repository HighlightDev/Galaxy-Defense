#include "CameraSceneProxy.h"

#include "Core/GameCore/ACamera.h"

using namespace Game;

namespace Graphics
{
   CameraSceneProxy::CameraSceneProxy(const ACamera* camera)
      : SceneProxyBase()
      , mEyeVector()
      , mViewMatrix()
   {
   }

   void CameraSceneProxy::UpdateViewMatrix(const glm::mat4& viewMatrix)
   {
      mViewMatrix = viewMatrix;
   }

   void CameraSceneProxy::UpdateEyeVector(const glm::vec3& eyeVector)
   {
      mEyeVector = eyeVector;
   }

   glm::mat4 CameraSceneProxy::GetViewMatrix() const
   {
      return mViewMatrix;
   }

   glm::vec3 CameraSceneProxy::GetEyeVector() const
   {
      return mEyeVector;
   }
}