#pragma once
#include "Component.h"
#include "Core/CommonCore/VariableWrapper.h"
#include "Core/GameCore/Components/Transform.h"

#include <glm/mat4x4.hpp>

namespace EngineCore
{
   class SceneComponent
       : public Component
   {
      using Base = Component;

   protected:
      bool bTransformationDirty;

      std::shared_ptr<Transform> mTransform;

      std::shared_ptr<EngineGOProperty<glm::vec3>> m_additionalRotationEuler;

      glm::mat4 m_relativeMatrix;

   public:
      bool bIsRootComponent = false;

      SceneComponent(const std::string &gameObjectName, const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale);

      ~SceneComponent();

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      virtual eComponentType GetComponentType() const override;

      /* This method works every time when this component has dirty transform */
      virtual void UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix = glm::mat4(1));

      void SetIsTransformationDirty(const bool isDirty);

      void SetTranslation(const glm::vec3 &translation);

      void SetRotator(const glm::quat &rotator);

      void SetScale(glm::vec3 scale, const bool bTriggerTransformUpdateEvent = true);

      void SetAdditionalRotation(const glm::vec3 &rotationEuler, const bool bTriggerTransformUpdateEvent = true);

      std::weak_ptr<Transform> GetTransformWeakPtr() const;

      bool GetIsTransformationDirty() const;

      glm::vec3 GetTranslation() const;

      glm::quat GetRotator() const;

      glm::vec3 GetRotationEuler() const;

      glm::vec3 GetScale() const;

      glm::mat4 GetRelativeMatrix() const;

      void AddTranslation(const glm::vec3 &offsetTranslation);
   };
}
