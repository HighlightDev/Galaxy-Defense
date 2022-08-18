#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

using namespace EngineMath;

namespace EngineCore
{
   SceneComponent::SceneComponent(const std::string &gameObjectName,
                                  const glm::vec3 &translation = glm::vec3(0.0f),
                                  const glm::vec3 &rotation = glm::vec3(0.0f),
                                  const glm::vec3 &scale = glm::vec3(0.0f))
       : Component(gameObjectName),
         m_TransformScale(std::make_shared<EngineGOProperty<glm::vec3>>(scale, "p_scale", [=](const glm::vec3 &scale)
                                                                        { SyncScale(scale); })),
         bTransformationDirty(true),
         mTransform(std::make_shared<Transform>(translation,
                                                glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z))),
                                                scale)),
         m_additionalRotationEuler(std::make_shared<EngineGOProperty<glm::vec3>>(glm::vec3(0.0f), "b_rotator",
                                                                                 [=](const glm::vec3 &rotator)
                                                                                 { SetIsTransformationDirty(true); })),
         m_relativeMatrix(1)
   {
      AddEngineProperty(m_additionalRotationEuler);
      AddEngineProperty(m_TransformScale);
   }

   SceneComponent::~SceneComponent()
   {
   }

   void SceneComponent::SyncScale(const glm::vec3& scale)
   {
      SetScale(scale);
   }

   void SceneComponent::Tick(const float deltaTime)
   {
      Component::Tick(deltaTime);

      if (!mIsEnabled)
         return;
   }

   void SceneComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
   }

   eComponentType SceneComponent::GetComponentType() const
   {
      return SCENE_COMPONENT;
   }

   void SceneComponent::AddTranslation(const glm::vec3 &offsetTranslation)
   {
      SetTranslation(mTransform->Translation + offsetTranslation);
   }

   void SceneComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      if (!mIsEnabled)
         return;

      // Update current relative matrix

      const glm::mat4 identityMatrix(1);
      m_relativeMatrix = identityMatrix;
      m_relativeMatrix *= parentRelativeMatrix;
      m_relativeMatrix *= glm::translate(identityMatrix, mTransform->Translation);
      m_relativeMatrix *= glm::scale(identityMatrix, mTransform->Scale);

      if (bIsRootComponent)
      {
         const glm::mat4 pitchRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler->GetValue().x), AXIS_RIGHT);
         const glm::mat4 yawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler->GetValue().y), AXIS_UP);
         const glm::mat4 rollRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler->GetValue().z), AXIS_FORWARD);

         m_relativeMatrix *= pitchRotation;
         m_relativeMatrix *= yawRotation;
         m_relativeMatrix *= rollRotation;
      }

      const auto nRotator = glm::normalize(mTransform->Rotator);
      m_relativeMatrix *= glm::toMat4(nRotator);

      SetIsTransformationDirty(false);
   }

   void SceneComponent::SetIsTransformationDirty(const bool isDirty)
   {
      bTransformationDirty = isDirty;
   }

   void SceneComponent::SetTranslation(const glm::vec3 &translation)
   {
      mTransform->Translation = translation;
      if (std::isnan(translation.x) || std::isnan(translation.y) || std::isnan(translation.z))
      {
         volatile auto b = 0;
      }
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetRotator(const glm::quat &rotator)
   {
      mTransform->Rotator = rotator;
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetScale(glm::vec3 scale)
   {
      mTransform->Scale = scale;
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetAdditionalRotation(const glm::vec3 &rotationEuler)
   {
      m_additionalRotationEuler->SetValue(rotationEuler);
      SetIsTransformationDirty(true);
   }

   std::weak_ptr<Transform> SceneComponent::GetTransformWeakPtr() const
   {
      return mTransform;
   }

   bool SceneComponent::GetIsTransformationDirty() const
   {
      return bTransformationDirty;
   }

   glm::vec3 SceneComponent::GetTranslation() const
   {
      return mTransform->Translation;
   }

   glm::quat SceneComponent::GetRotator() const
   {
      return mTransform->Rotator;
   }

   glm::vec3 SceneComponent::GetRotationEuler() const
   {
      return EngineMath::QuatToEulerAngles(GetRotator());
   }

   glm::vec3 SceneComponent::GetScale() const
   {
      return mTransform->Scale;
   }

   glm::mat4 SceneComponent::GetRelativeMatrix() const
   {
      return m_relativeMatrix;
   }

   glm::vec3 SceneComponent::GetHierarchyAccumulatedTranslation() const
   {
      glm::vec3 result(0.0f);

      if (auto ownerSp = GetOwner().lock())
      {
         IterateHierarchyUpCollectTranslation(ownerSp->GetParent(), result);
      }

      result += mTransform->Translation;

      return result;
   }

   glm::quat SceneComponent::GetHierarchyAccumulatedRotator() const
   {
      glm::quat result(glm::vec3(0, 0, 0));

      if (auto ownerSp = GetOwner().lock())
      {
         IterateHierarchyUpCollectRotator(ownerSp->GetParent(), result);
      }

      result *= mTransform->Rotator;

      return result;
   }

   void SceneComponent::IterateHierarchyUpCollectRotator(const std::weak_ptr<Actor> &currentOwnerWp, glm::quat &accumulatedRotator) const
   {
      if (auto currentOwnerSp = currentOwnerWp.lock())
      {
         IterateHierarchyUpCollectRotator(currentOwnerSp->GetParent(), accumulatedRotator);
         accumulatedRotator *= currentOwnerSp->GetRootComponent()->GetRotator();
      }
   }

   void SceneComponent::IterateHierarchyUpCollectTranslation(const std::weak_ptr<Actor> &currentOwnerWp, glm::vec3 &accumulatedTranslation) const
   {
      if (auto currentOwnerSp = currentOwnerWp.lock())
      {
         IterateHierarchyUpCollectTranslation(currentOwnerSp->GetParent(), accumulatedTranslation);
         accumulatedTranslation += currentOwnerSp->GetRootComponent()->GetTranslation();
      }
   }

}
