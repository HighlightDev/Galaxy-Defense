#include "ICamera.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/GlobalInputController.h"
#include <iostream>

using namespace EngineMath;

namespace Game
{

   ICamera::ICamera(const std::string& cameraName, const float initPitchDeg, const float initYawDeg)
      : GameObject(cameraName)
      , m_rotateSensetivity(0.08f)
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
      const int32_t x = GlobalInputController::GetInstance()->GetMouseDeltaX();
      const int32_t y = GlobalInputController::GetInstance()->GetMouseDeltaY();

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
}
