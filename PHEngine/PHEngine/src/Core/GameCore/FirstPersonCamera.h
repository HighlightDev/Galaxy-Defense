#pragma once

#include "ACamera.h"

#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

using namespace Event;

namespace Game
{
   class Scene;
	class FirstPersonCamera 
      : public ACamera
      , public MouseMovedEvent
      , public KeyboardButtonDownEvent
	{
	private:

		float m_cameraMoveSpeed;
      
		glm::vec3 m_firstPersonCameraPosition;

	public:

		FirstPersonCamera(const std::string& cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort, const float initPitchDeg, const float initYawDeg, glm::vec3 camPos);

		~FirstPersonCamera();

      virtual void Tick(const float DeltaTime) override;

      virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;

      virtual glm::vec3 GetEyeVector()  const override;

      virtual glm::vec3 GetTargetVector()  const override;

      virtual glm::vec3 GetLocalSpaceUpVector() const override;

      virtual std::string GetCameraTypeName() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual void ProcessEvent(const KeyboardButtonDownEvent::EventData_t& eventData) override;

      virtual void ProcessEvent(const MouseMovedEvent::EventData_t& eventData) override;

      void MoveCamera(int32_t direction);

      inline float GetCameraMoveSpeed()  const {

         return m_cameraMoveSpeed;
      }
	};

}
