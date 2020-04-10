#include "ICamera.h"
#include "Core/UtilityCore/EngineMath.h"
#include <iostream>

using namespace EngineMath;

namespace Game
{

   ICamera::ICamera(const std::string& cameraName, const float initPitchDeg, const float initYawDeg)
      : m_rotateSensetivity(0.08f)
      , mCameraName(cameraName)
      , m_localSpaceRightVector(std::move(glm::vec3(1, 0, 0)))
      , m_localSpaceUpVector(std::move(glm::vec3(0, 1, 0)))
      , m_localSpaceForwardVector(std::move(glm::vec3(0, 0, 1)))
      , m_eyeSpaceRightVector(std::move(glm::vec3(1, 0, 0)))
      , m_eyeSpaceForwardVector(std::move(glm::vec3(0, 0, 1)))
      , mPitchClampValue_min_max(glm::vec2(-80, 80))
      , mYaw(initYawDeg)
      , mPitch(std::clamp(initPitchDeg, mPitchClampValue_min_max.x, mPitchClampValue_min_max.y))
      , m_cameraType(CameraType::UNINITIALIZED)
	{
	}

	ICamera::~ICamera()
	{
	}

	void ICamera::Rotate()
	{
      const int32_t x = GlobalProperties::GetInstance()->GetInputData().GetMouseDeltaX();
      const int32_t y = GlobalProperties::GetInstance()->GetInputData().GetMouseDeltaY();

		UpdateRotationMatrix(-x, -y);
	}

   void ICamera::Tick(const float DeltaTime)
   {
   }

	void ICamera::UpdateRotationMatrix(int32_t deltaX, int32_t deltaY)
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

	glm::vec3 ICamera::LerpPosition(float t, float t1, float t2, const glm::vec3& position1, const glm::vec3& position2) const
	{
		glm::vec3 resultPosition = glm::vec3(0);

		float x_delta = t2 - t1;
		float x_zero_offset = t - t1;

		resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
		resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;
		resultPosition.z = ((position2.z - position1.z) / x_delta) * x_zero_offset + position1.z;

		return resultPosition;
	}

}
