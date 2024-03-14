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

   ACamera::ACamera(const std::string &cameraName,
                    const eCameraType cameraType,
                    std::shared_ptr<Scene> scene,
                    const ViewPortInfo &viewPort,
                    const std::shared_ptr<ViewProjectionInfo> &viewProjectionInfo,
                    const float initPitchDeg,
                    const float initYawDeg)
       : EngineObject(cameraName),
         m_rotateSensetivity(0.08f),
         mCameraName(cameraName),
         mViewProjectionInfo(viewProjectionInfo),
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
      WindowSizeChangedGameThreadEvent::GetInstance()->RemoveListener(WindowSizeChangedGameThreadEvent::GetInstanceId());
   }

   void ACamera::Initialize()
   {
      WindowSizeChangedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
   }

   void ACamera::SetCameraProxyId(const size_t proxyId)
   {
      mCameraProxyId = proxyId;
   }

   size_t ACamera::GetCameraProxyId() const
   {
      return mCameraProxyId;
   }

   void ACamera::SetIsCameraProxyReady(const bool isReady)
   {
      bIsCameraProxyReady.store(isReady, std::memory_order::memory_order_seq_cst);
   }

   bool ACamera::IsCameraProxyReady() const
   {
      return bIsCameraProxyReady.load(std::memory_order::memory_order_seq_cst);
   }

   void ACamera::SetRotation(const int32_t deltaX, const int32_t deltaY)
   {
      UpdateRotationMatrix(-deltaX, -deltaY);
   }

   void ACamera::UpdateCameraProxyData()
   {
      if (auto sceneSp = mScene.lock())
      {
         if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            static constexpr uint64_t functionId = Hash64_CT("ACamera::UpdateCameraProxyData");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId,
                                                                                [cameraPtr = this,
                                                                                 eyeVector = GetEyeVector(),
                                                                                 viewMatrix = GetViewMatrix(),
                                                                                 eyeForwardVector = GetEyeSpaceForwardVector(),
                                                                                 eyeRightVector = GetEyeSpaceRightVector(),
                                                                                 eyeUpVector = GetLocalSpaceUpVector(),
                                                                                 cameraProxyId = mCameraProxyId,
                                                                                 sceneRendererSp]()
                                                                                {
                                                                                   if (const auto &sceneViewSp = sceneRendererSp->GetSceneViewByProxyId(cameraProxyId))
                                                                                   {
                                                                                      assert(sceneViewSp);
                                                                                      const auto &cameraProxy = sceneViewSp->GetCameraProxy();
                                                                                      cameraProxy->UpdateEyeVector(eyeVector);
                                                                                      cameraProxy->UpdateViewMatrix(viewMatrix);
                                                                                      cameraProxy->SetForwardVector(eyeForwardVector);
                                                                                      cameraProxy->SetRightVector(eyeRightVector);
                                                                                      cameraProxy->SetUpVector(eyeUpVector);

                                                                                      cameraPtr->OnCameraSceneProxyDataUpdated();
                                                                                   }
                                                                                });
         }
      }
   }

   void ACamera::OnCameraSceneProxyDataUpdated()
   {
      Event::CameraTransformChangedGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, this);
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

      if (bTransformationDirty && bIsCameraProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         bTransformationDirty = false;
         OnTransformationUpdated();
         UpdateCameraProxyData();
      }
   }

   void ACamera::ProcessEvent(const WindowSizeChangedGameThreadEvent::EventData_t &data)
   {
      const auto newViewPortInfo = std::get<0>(data);
      mViewPort = newViewPortInfo;

      if (auto sceneSp = mScene.lock())
      {
         if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            static constexpr uint64_t functionId = Hash64_CT("ACamera::WindowSizeChangedGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId,
                                                                                [newViewPortInfo = mViewPort,
                                                                                 cameraProxyId = mCameraProxyId,
                                                                                 sceneRendererSp]()
                                                                                {
                                                                                   if (const auto &sceneViewSp = sceneRendererSp->GetSceneViewByProxyId(cameraProxyId))
                                                                                   {
                                                                                      assert(sceneViewSp);
                                                                                      const auto &cameraProxy = sceneViewSp->GetCameraProxy();
                                                                                      cameraProxy->SetViewPortInfo(newViewPortInfo);
                                                                                   }
                                                                                });
         }
      }
   }

   void ACamera::PostLevelInit()
   {
      if (mPlanarReflectionComponent)
      {
         mPlanarReflectionComponent->PostLevelInit();
      }
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

   const std::shared_ptr<ViewProjectionInfo>& ACamera::GetViewProjectionInfo() const
   {
      return mViewProjectionInfo;
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
         clippedSpacePosition = sceneSp->GetConvertedToClippedSpacePosition(mCameraProxyId, worldPosition);
      }

      return clippedSpacePosition;
   }

   std::optional<CameraFrustum> ACamera::GetCameraFrustum() const
   {
      std::optional<CameraFrustum> result{std::nullopt};

      if (auto sceneSp = mScene.lock())
      {
         result = sceneSp->GetCameraFrustum(mCameraProxyId);
      }

      return result;
   }
}
