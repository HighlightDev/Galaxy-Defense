#pragma once
#include "ICamera.h"
#include "Actor.h"

namespace Game
{
   class ThirdPersonCamera :
      public ICamera
   {

      float m_distanceFromTargetToCamera;

      std::shared_ptr<Actor> m_thirdPersonTarget;

      glm::vec3 m_actualTargetVector;

      float m_lerpTimeElapsed = 0.0f;

      float m_timeForInterpolation = 0.15f;

      bool m_bThirdPersonTargetTransformationDirty = false;

   public:

      float m_maxDistanceFromTargetToCamera;

      ThirdPersonCamera();

      ThirdPersonCamera(glm::vec3 localSpaceForwardVector, float camDistanceToThirdPersonTarget);

      virtual ~ThirdPersonCamera();

      virtual void Tick(const float DeltaTime) override;

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

      void SetThirdPersonTargetTransformationDirty();
   };
}

