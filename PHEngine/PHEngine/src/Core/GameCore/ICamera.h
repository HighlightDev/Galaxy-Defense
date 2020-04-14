#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "Core/GameCore/GameObject.h"
#include "Core/GameCore/ITickable.h"

namespace Game
{

	class ICamera
      : public GameObject
      , public ITickable
	{
   public:

      enum class CameraType
      {
         FIRST_PERSON,
         THIRD_PERSON,
         UNINITIALIZED
      };

   private:

		float m_rotateSensetivity;

      std::string mCameraName;

	protected:

		glm::vec3 m_localSpaceRightVector;
		glm::vec3 m_localSpaceUpVector;
		glm::vec3 m_localSpaceForwardVector;
		glm::vec3 m_eyeSpaceRightVector;
		glm::vec3 m_eyeSpaceForwardVector;
		glm::mat3 m_rotationMatrix;

      const glm::vec2 mPitchClampValue_min_max;

      float mYaw;
      float mPitch;
		
		bool bTransformationDirty = false;
      CameraType m_cameraType;

		virtual void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY);

	public:

		float CameraCollisionSphereRadius = 8.0f;

		ICamera(const std::string& cameraName, const float initPitchDeg, const float initYawDeg);

		virtual ~ICamera();

		virtual void Tick(const float DeltaTime) override;

		virtual glm::vec3 GetEyeVector() const = 0;

		virtual glm::vec3 GetTargetVector() const = 0;

		virtual glm::vec3 GetLocalSpaceUpVector() const = 0;

      inline std::string GetCameraName() const {
         return mCameraName;
      }

      inline CameraType GetCameraType() const {

         return m_cameraType;
      }

		inline void SetLocalSpaceUpVector(glm::vec3& upVector)
		{
			m_localSpaceUpVector = upVector;
		}

		inline void SetLocalSpaceForwardVector(glm::vec3& forwardVector)
		{
			m_localSpaceForwardVector = forwardVector;
		}

		inline void SetLocalSpaceRightVector(glm::vec3& rightVector)
		{
			m_localSpaceRightVector = rightVector;
		}

		inline void SetCameraSensetivity(float rotateSensetivity)
		{
			m_rotateSensetivity = rotateSensetivity;
		}

		inline float GetCameraSensetivity() const
		{
			return m_rotateSensetivity;
		}

		inline glm::vec3 GetLocalSpaceRightVector() const
		{
			return m_localSpaceRightVector;
		}

		inline glm::vec3 GetLocalSpaceForwardVector() const
		{
			return m_localSpaceForwardVector;
		}

		inline glm::vec3 GetEyeSpaceForwardVector() const
		{
			return m_eyeSpaceForwardVector;
		}

		inline glm::vec3 GetEyeSpaceRightVector() const
		{
			return m_eyeSpaceRightVector;
		}

		inline glm::mat4 GetViewMatrix() const
		{
			return glm::lookAt(GetEyeVector(), GetTargetVector(), GetLocalSpaceUpVector());
		}

		inline glm::mat3 GetRotationMatrix() const
		{
			return m_rotationMatrix;
		}

      inline float GetRotationYaw() const {
         return mYaw;
      }

      inline float GetRotationPitch() const {
         return mPitch;
      }

		void Rotate();

		glm::vec3 LerpPosition(float t, float t1, float t2, const glm::vec3& position1, const glm::vec3& position2) const;

		/*inline FSphere GetCameraCollisionSphere()
		{
			return new FSphere(GetEyeVector(), CameraCollisionSphereRadius);
		}*/

		//void SetCollisionHeadUnit(CollisionHeadUnit collisionHeadUnit);
	};

}

