#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "Core/GameCore/GameObject.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GameCore/Serialize/ISerializable.h"

using namespace Graphics;

namespace Game
{
   class Scene;
   class PlanarReflectionComponent;

   class ACamera
      : public GameObject
      , public ITickable
      , public ISerializable
   {
   public:

      enum CameraType
      {
         SECONDARY_FIRST_PERSON_CAMERA = 0x0001, // not fully implemented
         MAIN_FIRST_PERSON_CAMERA = (SECONDARY_FIRST_PERSON_CAMERA | 0x0010), // not fully implemented
         SECONDARY_THIRD_PERSON_CAMERA = 0x0100,
         MAIN_THIRD_PERSON_CAMERA = (SECONDARY_THIRD_PERSON_CAMERA | 0x1000),
         UNINITIALIZED = 0x0,
      };

   private:

      float m_rotateSensetivity;

      std::string mCameraName;

      ViewPerspectiveInfo mViewPerspectiveInfo;

   protected:

      std::weak_ptr<Scene> mScene;

      std::shared_ptr<PlanarReflectionComponent> mPlanarReflectionComponent;

      ViewPortInfo mViewPort;

      glm::vec3 m_localSpaceRightVector;

      glm::vec3 m_localSpaceUpVector;

      glm::vec3 m_localSpaceForwardVector;

      glm::vec3 m_eyeSpaceRightVector;

      glm::vec3 m_eyeSpaceForwardVector;

      const glm::vec2 mPitchClampValue_min_max;

      float mYaw;

      float mPitch;

      bool bTransformationDirty = false;

      CameraType m_cameraType;

   public:

      size_t SceneProxyId = 0;

   public:

      ACamera(const std::string& cameraName, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort, const float initPitchDeg, const float initYawDeg);

      virtual ~ACamera();

      virtual void Tick(const float DeltaTime) override;

      virtual void PostLevelInit();

      virtual glm::vec3 GetEyeVector() const = 0;

      virtual glm::vec3 GetTargetVector() const = 0;

      virtual glm::vec3 GetLocalSpaceUpVector() const = 0;

      virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const = 0;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;

      std::shared_ptr<PlanarReflectionComponent> GetPlanarReflectionComponent() const;

      std::string GetCameraName() const;

      CameraType GetCameraType() const;

      virtual std::string GetCameraTypeName() const = 0;

      void SetPlanarReflectionComponent(std::shared_ptr<PlanarReflectionComponent> planarReflectionComponent);

      void SetLocalSpaceUpVector(glm::vec3& upVector);

      void SetLocalSpaceForwardVector(glm::vec3& forwardVector);

      void SetLocalSpaceRightVector(glm::vec3& rightVector);

      void SetCameraSensetivity(float rotateSensetivity);

      float GetCameraSensetivity() const;

      glm::vec3 GetLocalSpaceRightVector() const;

      glm::vec3 GetLocalSpaceForwardVector() const;

      glm::vec3 GetEyeSpaceForwardVector() const;

      glm::vec3 GetEyeSpaceRightVector() const;

      glm::mat4 GetViewMatrix() const;

      float GetRotationYaw() const;

      float GetRotationPitch() const;

      ViewPortInfo GetViewPort() const;

      ViewPerspectiveInfo GetViewPerspectiveInfo() const;

   protected:

      void SetRotation(const int32_t deltaX, const int32_t deltaY);

      virtual void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY);

   private:

      void UpdateCameraProxyData(const float DeltaTime);
   };

}

