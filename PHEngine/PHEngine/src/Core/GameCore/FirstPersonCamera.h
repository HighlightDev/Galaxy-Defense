#pragma once

#include "ACamera.h"

namespace Game
{
   class Scene;
	class FirstPersonCamera 
      : public ACamera
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

      void MoveCamera(int32_t direction);

      inline float GetCameraMoveSpeed()  const {

         return m_cameraMoveSpeed;
      }
	};

}
