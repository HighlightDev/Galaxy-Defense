#pragma once
#include "ICamera.h"
#include "Actor.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

using namespace Event;

namespace Game
{
   class ThirdPersonCamera
      : public ICamera
      , public PlayerMovedEvent
   {

      float m_distanceFromTargetToCamera;

      std::shared_ptr<Actor> m_thirdPersonTarget;

      glm::vec3 m_actualTargetVector;

      float m_lerpTimeElapsed = 0.0f;

      float m_timeForInterpolation = 0.55f;

      bool m_bThirdPersonTargetTransformationDirty = false;

   public:

      float m_maxDistanceFromTargetToCamera;

      ThirdPersonCamera(const std::string& cameraName, const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget);

      virtual ~ThirdPersonCamera();

      virtual void Tick(const float DeltaTime) override;

      virtual void ProcessEvent(const PlayerMovedEvent::EventData_t& data);

      virtual void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY) override;

      void SetMaxDistanceFromTargetToCamera(float maxDistanceFromTargetToCamera);

      float GetMaxDistanceFromTargetToCamera() const;

      float GetTimeForInterpolation() const;

      void SetTimeForInterpolation(float timeForInterpolation);

      virtual glm::vec3 GetLocalSpaceUpVector() const override;

      virtual glm::vec3 GetEyeVector() const override;

      virtual glm::vec3 GetTargetVector() const override;

      void SetDistanceFromTargetToCamera(float distanceFromTargetToCamera);

      float GetDistanceFromTargetToCamera() const;

      std::shared_ptr<Actor> GetThirdPersonTarget() const;

      void SetThirdPersonTarget(std::shared_ptr<Actor> thirdPersonTarget);
   };
}

