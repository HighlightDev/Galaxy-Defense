#pragma once

#include "ICamera.h"

namespace Game
{

	class FirstPersonCamera : public ICamera
	{
	private:

		float m_cameraMoveSpeed;
		glm::vec3 m_firstPersonCameraPosition;

	public:

		FirstPersonCamera(const std::string& cameraName, const float initPitchDeg, const float initYawDeg, glm::vec3 camPos);

		~FirstPersonCamera();

      virtual void Tick(const float DeltaTime) override;

      virtual glm::vec3 GetEyeVector()  const override;

      virtual glm::vec3 GetTargetVector()  const override;

      virtual glm::vec3 GetLocalSpaceUpVector() const override;

      void MoveCamera(int32_t direction);

      inline float GetCameraMoveSpeed()  const {

         return m_cameraMoveSpeed;
      }
	};

}
