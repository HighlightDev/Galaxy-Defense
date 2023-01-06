#include "ThirdPersonCamera.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"

#include <algorithm>

#undef max
#undef min

namespace EngineCore
{

   ThirdPersonCamera::ThirdPersonCamera(const std::string &cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort,
                                        const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget, const glm::vec3 &thirdPersonTargetOffset)
       : ACamera(cameraName, cameraType, scene, viewPort, initPitchDeg, initYawDeg), PlayerMovedEvent(), mThirdPersonTargetGOName(""), bThirdPersonTargetDeferredDirty(false), m_thirdPersonTargetOffset(thirdPersonTargetOffset)
   {
      PlayerMovedEvent::GetInstance()->AddListener(this);
      SetMaxDistanceFromTargetToCamera(camDistanceToThirdPersonTarget);
      m_distanceFromTargetToCamera = camDistanceToThirdPersonTarget;
   }

   ThirdPersonCamera::~ThirdPersonCamera()
   {
      PlayerMovedEvent::GetInstance()->RemoveListener(this);
   }

   void ThirdPersonCamera::ProcessEvent(const PlayerMovedEvent::EventData_t &data)
   {
      m_bThirdPersonTargetTransformationDirty = true;
      m_lerpTimeElapsed = 0.0f;
   }

   void ThirdPersonCamera::UpdateRotationMatrix(int32_t deltaX, int32_t deltaY)
   {
      ACamera::UpdateRotationMatrix(deltaX, deltaY);
      Event::CameraTransformChangedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, this);
   }

   void ThirdPersonCamera::PostLevelInit()
   {
      ACamera::PostLevelInit();

      ProcessDeferredThirdPersonTarget();
   }

   void ThirdPersonCamera::Tick(const float DeltaTime)
   {
      ACamera::Tick(DeltaTime);

      float clampedDeltaTime = std::max(DeltaTime, 0.03f);

      if (m_bThirdPersonTargetTransformationDirty)
      {
         m_lerpTimeElapsed = std::min(m_lerpTimeElapsed + clampedDeltaTime, m_timeForInterpolation);

         glm::vec3 finalTargetVector = m_thirdPersonTarget->GetRootComponent()->GetTranslation();
         m_actualTargetVector = EngineMath::LerpVec3(m_lerpTimeElapsed, 0.0f, m_timeForInterpolation, m_actualTargetVector, finalTargetVector);

         SetTransformationDirty();

         // If camera is at final position
         if (EngineMath::FloatsNearEqual(m_lerpTimeElapsed, m_timeForInterpolation))
         {
            m_lerpTimeElapsed = 0.0f;
            m_bThirdPersonTargetTransformationDirty = false;
         }
      }
   }

   void ThirdPersonCamera::SetMaxDistanceFromTargetToCamera(float maxDistanceFromTargetToCamera)
   {
      m_maxDistanceFromTargetToCamera = maxDistanceFromTargetToCamera;
      SetTransformationDirty();
   }

   float ThirdPersonCamera::GetMaxDistanceFromTargetToCamera() const
   {
      return m_maxDistanceFromTargetToCamera;
   }

   float ThirdPersonCamera::GetTimeForInterpolation() const
   {
      return m_timeForInterpolation;
   }

   void ThirdPersonCamera::SetTimeForInterpolation(float timeForInterpolation)
   {
      m_timeForInterpolation = timeForInterpolation;
   }

   glm::vec3 ThirdPersonCamera::GetLocalSpaceUpVector() const
   {
      return m_localSpaceUpVector;
   }

   glm::vec3 ThirdPersonCamera::GetEyeVector() const
   {
      assert(m_thirdPersonTarget);
      return GetTargetVector() - (GetEyeSpaceForwardVector() * m_distanceFromTargetToCamera);
   }

   glm::vec3 ThirdPersonCamera::GetTargetVector() const
   {
      assert(m_thirdPersonTarget);
      return m_actualTargetVector + m_thirdPersonTargetOffset; // attach to "head"
   }

   void ThirdPersonCamera::SetDistanceFromTargetToCamera(float distanceFromTargetToCamera)
   {
      m_distanceFromTargetToCamera = distanceFromTargetToCamera;
      SetTransformationDirty();
   }

   void ThirdPersonCamera::Zoom(eMouseScrollDirection zoomDirection, float zoomPower)
   {
      switch (zoomDirection)
      {
      case eMouseScrollDirection::ZoomIn:
      {
         SetDistanceFromTargetToCamera(std::max(m_distanceFromTargetToCamera - zoomPower, sCameraMinDistance));
         break;
      }
      case eMouseScrollDirection::ZoomOut:
      {
         SetDistanceFromTargetToCamera(std::min(m_distanceFromTargetToCamera + zoomPower, sCameraMaxDistance));
         break;
      }

      default:
         break;
      }
   }

   float ThirdPersonCamera::GetDistanceFromTargetToCamera() const
   {
      return m_distanceFromTargetToCamera;
   }

   glm::vec3 ThirdPersonCamera::GetThirdPersonTargetOffset() const
   {
      return m_thirdPersonTargetOffset;
   }

   std::shared_ptr<Actor> ThirdPersonCamera::GetThirdPersonTarget() const
   {
      return m_thirdPersonTarget;
   }

   std::shared_ptr<CameraSceneProxy> ThirdPersonCamera::CreateSceneProxy() const
   {
      if (eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType)
      {
         return std::make_shared<MainCameraSceneProxy>(this);
      }
      else
      {
         return std::make_shared<CameraSceneProxy>(this);
      }
   }

   std::string ThirdPersonCamera::GetCameraTypeName() const
   {
      return eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType ? "MainThirdPersonCamera" : "ThirdPersonCamera";
   }

   void ThirdPersonCamera::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto cameraData = SerializeHelper::GetSerializedDataCamera(this);
      dataContainer.Cameras.emplace_back(cameraData);
   }

   void ThirdPersonCamera::SetThirdPersonTargetDeferred(const std::string &targetGameObjectName)
   {
      bThirdPersonTargetDeferredDirty = true;
      mThirdPersonTargetGOName = targetGameObjectName;
   }

   void ThirdPersonCamera::SetThirdPersonTarget(std::shared_ptr<Actor> thirdPersonTarget)
   {
      assert(!bThirdPersonTargetDeferredDirty);

      m_thirdPersonTarget = thirdPersonTarget;
      m_actualTargetVector = thirdPersonTarget->GetRootComponent()->GetTranslation();

      ACamera::UpdateRotationMatrix(0, 0);
   }

   void ThirdPersonCamera::ProcessDeferredThirdPersonTarget()
   {
      if (auto sceneSp = mScene.lock())
      {
         assert(mThirdPersonTargetGOName != "");
         const auto &actor = sceneSp->GetActorByName(mThirdPersonTargetGOName);
         bThirdPersonTargetDeferredDirty = false;
         SetThirdPersonTarget(actor);
      }
   }
}
