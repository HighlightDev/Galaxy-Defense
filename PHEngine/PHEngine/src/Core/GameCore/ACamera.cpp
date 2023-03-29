#include "ACamera.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

#include <algorithm>

using namespace EngineMath;
using namespace IO;
using namespace Graphics::Renderer;

namespace EngineCore
{

   ACamera::ACamera(const std::string &cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg, const float initYawDeg)
       : EngineObject(cameraName),
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
   }

   ACamera::~ACamera()
   {
   }

   void ACamera::SetRotation(const int32_t deltaX, const int32_t deltaY)
   {
      UpdateRotationMatrix(-deltaX, -deltaY);
   }

   bool ACamera::UpdateCameraProxyData()
   {
      auto updateSuccessfull = false;
      if (auto sceneSp = mScene.lock())
      {
         if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            static constexpr uint64_t functionId = Hash64_CT("ACamera::UpdateCameraProxyData");

            const auto &sceneViewSp = sceneRendererSp->GetSceneViewByProxyId(SceneProxyId);
            if (sceneViewSp)
            {
               updateSuccessfull = true;
               sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId,
                  [cameraPtr = this,
                   eyeVector = GetEyeVector(),
                   viewMatrix = GetViewMatrix(),
                   eyeForwardVector = GetEyeSpaceForwardVector(),
                   eyeRightVector = GetEyeSpaceRightVector(),
                   eyeUpVector = GetLocalSpaceUpVector(),
                   sceneViewSp,
                   sceneRendererSp
                   ]() {
                  const auto& cameraProxy = sceneViewSp->GetCameraProxy();
                  cameraProxy->UpdateEyeVector(eyeVector);
                  cameraProxy->UpdateViewMatrix(viewMatrix);
                  cameraProxy->SetForwardVector(eyeForwardVector);
                  cameraProxy->SetRightVector(eyeRightVector);
                  cameraProxy->SetUpVector(eyeUpVector);

                  static constexpr uint64_t innerFunctionId = Hash64_CT("ACamera::UpdateCameraProxyData");
                  sceneRendererSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, cameraProxy->GetSceneProxyId(), innerFunctionId, [cameraPtr]() {
                     cameraPtr->OnCameraSceneProxyDataUpdated(); 
                  }); 
               });
            }
            else
            {
               LogInfo("ACamera::UpdateCameraProxyData => Error! Current proxy index doesn't exist on RT. Proxy index = ", SceneProxyId);
            }
         }
      }
      return updateSuccessfull;
   }

   void ACamera::OnCameraSceneProxyDataUpdated()
   {
      OnTransformationUpdated();
      Event::CameraTransformChangedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, this);
   }

   void ACamera::OnTransformationUpdated()
   {
   }

   void ACamera::SetTransformationDirty()
   {
      bTransformationDirty = true;
   }

   void ACamera::Tick(const float DeltaTime)
   {
      if (mPlanarReflectionComponent)
      {
         mPlanarReflectionComponent->Tick(DeltaTime);
      }

      if (bTransformationDirty)
      {
         bTransformationDirty = !UpdateCameraProxyData();
      }
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

      SetTransformationDirty();
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

   std::optional<CameraFrustum> ACamera::GetCameraFrustum() const
   {
      std::optional<CameraFrustum> result{std::nullopt};

      if (auto sceneSp = mScene.lock())
      {
         result = sceneSp->GetCameraFrustum(SceneProxyId);
      }

      return result;
   }
}
