#include "ACamera.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"

#include "Core/AudioCore/SoundDevice.h"

#include <algorithm>

using namespace EngineMath;
using namespace IO;

namespace EngineCore
{

   ACamera::ACamera(const std::string &cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg, const float initYawDeg)
       : GameObject(cameraName),
         m_rotateSensetivity(0.08f),
         mCameraName(cameraName),
         mViewPerspectiveInfo(DEG_TO_RAD(60), 16.0f / 9.0f, 1, 1000),
         mScene(scene), mPlanarReflectionComponent(nullptr),
         mViewPort(viewPort), m_localSpaceRightVector(1, 0, 0),
         m_localSpaceUpVector(0, 1, 0),
         m_localSpaceForwardVector(0, 0, 1),
         m_eyeSpaceRightVector(1, 0, 0),
         m_eyeSpaceForwardVector(0, 0, 1),
         mPitchClampValue_min_max(-80, 80),
         mPitch(std::clamp(initPitchDeg, mPitchClampValue_min_max.x, mPitchClampValue_min_max.y)),
         mYaw(initYawDeg),
         m_cameraType(cameraType)
   {
      SoundDevice::GetInstance();
   }

   ACamera::~ACamera()
   {
   }

   void ACamera::SetRotation(const int32_t deltaX, const int32_t deltaY)
   {
      UpdateRotationMatrix(-deltaX, -deltaY);
   }

   void ACamera::UpdateCameraProxyData(const float DeltaTime)
   {
      static const uint64_t functionId = Hash("ACamera: Update camera proxy data.");

      if (bTransformationDirty)
      {
         if (auto sceneSp = mScene.lock())
         {
            sceneSp->UpdateCameraSceneProxyData_OnRenderThread(SceneProxyId, GetObjectId(), functionId, this);
         }
      }
   }

   void ACamera::Tick(const float DeltaTime)
   {
      if (mPlanarReflectionComponent)
         mPlanarReflectionComponent->Tick(DeltaTime);

      UpdateCameraProxyData(DeltaTime);
   }

   void ACamera::PostLevelInit()
   {
      if (mPlanarReflectionComponent)
         mPlanarReflectionComponent->PostLevelInit();
   }

   void ACamera::UpdateRotationMatrix(int32_t deltaX, int32_t deltaY)
   {
      if (std::abs(deltaY) > 500)
         return;

      mYaw += (deltaX * m_rotateSensetivity);
      mPitch -= (deltaY * m_rotateSensetivity);

      // restrain angle of pitch
      mPitch = std::clamp(mPitch, mPitchClampValue_min_max.x, mPitchClampValue_min_max.y);

      glm::mat4 rotatePitch = glm::mat4(1);
      rotatePitch = glm::rotate(rotatePitch, DEG_TO_RAD(mPitch), m_localSpaceRightVector);

      glm::mat4 rotateYaw = glm::mat4(1);
      rotateYaw = glm::rotate(rotateYaw, DEG_TO_RAD(mYaw), m_localSpaceUpVector);

      glm::mat4 totalRotateMatrix = glm::mat4(1);
      totalRotateMatrix *= rotateYaw;
      totalRotateMatrix *= rotatePitch;

      m_eyeSpaceForwardVector = totalRotateMatrix * glm::vec4(m_localSpaceForwardVector, 0.0);
      m_eyeSpaceRightVector = totalRotateMatrix * glm::vec4(m_localSpaceRightVector, 0.0);

      bTransformationDirty = true;
   }

   std::string ACamera::GetCameraName() const
   {
      return mCameraName;
   }

   eCameraType ACamera::GetCameraType() const
   {
      return m_cameraType;
   }

   void ACamera::SetLocalSpaceUpVector(glm::vec3 &upVector)
   {
      m_localSpaceUpVector = upVector;
   }

   void ACamera::SetLocalSpaceForwardVector(glm::vec3 &forwardVector)
   {
      m_localSpaceForwardVector = forwardVector;
   }

   void ACamera::SetLocalSpaceRightVector(glm::vec3 &rightVector)
   {
      m_localSpaceRightVector = rightVector;
   }

   void ACamera::SetCameraSensetivity(float rotateSensetivity)
   {
      m_rotateSensetivity = rotateSensetivity;
   }

   float ACamera::GetCameraSensetivity() const
   {
      return m_rotateSensetivity;
   }

   glm::vec3 ACamera::GetLocalSpaceRightVector() const
   {
      return m_localSpaceRightVector;
   }

   glm::vec3 ACamera::GetLocalSpaceForwardVector() const
   {
      return m_localSpaceForwardVector;
   }

   glm::vec3 ACamera::GetEyeSpaceForwardVector() const
   {
      return m_eyeSpaceForwardVector;
   }

   glm::vec3 ACamera::GetEyeSpaceRightVector() const
   {
      return m_eyeSpaceRightVector;
   }

   glm::mat4 ACamera::GetViewMatrix() const
   {
      return glm::lookAt(GetEyeVector(), GetTargetVector(), GetLocalSpaceUpVector());
   }

   float ACamera::GetRotationYaw() const
   {
      return mYaw;
   }

   float ACamera::GetRotationPitch() const
   {
      return mPitch;
   }

   ViewPortInfo ACamera::GetViewPort() const
   {
      return mViewPort;
   }

   ViewPerspectiveInfo ACamera::GetViewPerspectiveInfo() const
   {
      return mViewPerspectiveInfo;
   }

   void ACamera::SetPlanarReflectionComponent(std::shared_ptr<PlanarReflectionComponent> planarReflectionComponent)
   {
      assert(!mPlanarReflectionComponent);
      mPlanarReflectionComponent = planarReflectionComponent;
   }

   std::shared_ptr<PlanarReflectionComponent> ACamera::GetPlanarReflectionComponent() const
   {
      return mPlanarReflectionComponent;
   }

   glm::vec4 ACamera::GetConvertedToClippedSpacePosition(const glm::vec4 &worldPosition)
   {
      glm::vec4 clippedSpacePosition = worldPosition;

      if (auto sceneSp = mScene.lock())
      {
         clippedSpacePosition = sceneSp->GetConvertedToClippedSpacePosition(SceneProxyId, worldPosition);
      }

      return clippedSpacePosition;
   }
}
