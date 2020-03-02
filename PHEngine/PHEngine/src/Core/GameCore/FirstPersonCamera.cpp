#include "FirstPersonCamera.h"

namespace Game
{

   FirstPersonCamera::FirstPersonCamera(const std::string& cameraName, glm::vec3 forwardVec, glm::vec3 camPos)
      : ICamera(cameraName)
      , m_firstPersonCameraPosition(camPos)
      , m_cameraMoveSpeed(0.1f)
   {
      m_cameraType = ICamera::CameraType::FIRST_PERSON;
   }

   FirstPersonCamera::~FirstPersonCamera()
   {

   }

   glm::vec3 FirstPersonCamera::GetEyeVector()  const
   {
      return m_firstPersonCameraPosition;
   }

   glm::vec3 FirstPersonCamera::GetTargetVector()  const
   {
      auto targetVec = m_firstPersonCameraPosition + m_eyeSpaceForwardVector * m_cameraMoveSpeed;
      return targetVec;
   }

   glm::vec3 FirstPersonCamera::GetLocalSpaceUpVector() const
   {
      return m_localSpaceUpVector;
   }

   void FirstPersonCamera::Tick(const float DeltaTime)
   {
   }

   void FirstPersonCamera::MoveCamera(int32_t direction)
   {
      switch (direction)
      {
         case 0: m_firstPersonCameraPosition += GetEyeSpaceForwardVector() * m_cameraMoveSpeed; break;
         case 1: m_firstPersonCameraPosition -= GetEyeSpaceForwardVector() * m_cameraMoveSpeed; break;
         case 3: m_firstPersonCameraPosition -= GetEyeSpaceRightVector() * m_cameraMoveSpeed; break;
         case 4: m_firstPersonCameraPosition += GetEyeSpaceRightVector() * m_cameraMoveSpeed; break;
      }
   }
}

