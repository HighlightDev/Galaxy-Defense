#pragma once
#include "ACamera.h"
#include "Actor.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

using namespace Event;

namespace Game
{
   class Scene;

   class ThirdPersonCamera
      : public ACamera
      , public PlayerMovedEvent
   {

   protected:

      float m_distanceFromTargetToCamera;

      std::shared_ptr<Actor> m_thirdPersonTarget;

      glm::vec3 m_actualTargetVector;

      glm::vec3 m_thirdPersonTargetOffset;

      float m_lerpTimeElapsed = 0.0f;

      float m_timeForInterpolation = 0.55f;

      bool m_bThirdPersonTargetTransformationDirty = false;

      std::string mThirdPersonTargetGOName;

      bool bThirdPersonTargetDeferredDirty;

   public:

      float m_maxDistanceFromTargetToCamera;

      ThirdPersonCamera(const std::string& cameraName, std::shared_ptr<Scene> scene,
         const ViewPortInfo& viewPort, const float initPitchDeg, const float initYawDeg,
         const float camDistanceToThirdPersonTarget, const glm::vec3& thirdPersonTargetOffset = glm::vec3());

      virtual ~ThirdPersonCamera();

      virtual void Tick(const float DeltaTime) override;

      virtual void ProcessEvent(const typename PlayerMovedEvent::EventData_t& data);

      virtual void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY) override;

      virtual void PostLevelInit() override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      void SetMaxDistanceFromTargetToCamera(float maxDistanceFromTargetToCamera);

      float GetMaxDistanceFromTargetToCamera() const;

      float GetTimeForInterpolation() const;

      void SetTimeForInterpolation(float timeForInterpolation);

      virtual std::string GetCameraTypeName() const override;

      virtual glm::vec3 GetLocalSpaceUpVector() const override;

      virtual glm::vec3 GetEyeVector() const override;

      virtual glm::vec3 GetTargetVector() const override;

      virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;

      void SetDistanceFromTargetToCamera(float distanceFromTargetToCamera);

      float GetDistanceFromTargetToCamera() const;

      glm::vec3 GetThirdPersonTargetOffset() const;

      std::shared_ptr<Actor> GetThirdPersonTarget() const;

      void SetThirdPersonTargetDeferred(const std::string& targetGameObjectName);

      void SetThirdPersonTarget(std::shared_ptr<Actor> thirdPersonTarget);

   private:

      void ProcessDeferredThirdPersonTarget();
   };
}

