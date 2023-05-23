#include "GhostPhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineMath;

namespace EnginePhysics
{
   GhostPhysicsComponent::GhostPhysicsComponent(const PhysicsComponentData &data)
       : PhysicsComponent(data),
         m_HitBoxScale(std::make_shared<EngineObjectProperty<glm::vec3>>(glm::vec3(1.0f),
                                                                             "p_hitBoxScale",
                                                                             [this](const glm::vec3 &value)
                                                                             { SyncHitBoxScale(value); }))
   {
      AddEngineProperty(m_HitBoxScale);
   }

   GhostPhysicsComponent::~GhostPhysicsComponent()
   {
   }

   void GhostPhysicsComponent::Tick(const float deltaTime)
   {
      Component::Tick(deltaTime);

      if (const auto &spOwner = GetOwner().lock())
      {
         const auto rootComponentSp = spOwner->GetRootComponent();
         const glm::vec3 translation = rootComponentSp->GetHierarchyAccumulatedTranslation();
         const glm::quat rotator = rootComponentSp->GetHierarchyAccumulatedRotator();

         mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      }

      bool bIsDirty;

      mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty, deltaTime);
      bIsTransformationDirty = bIsDirty;
   }

   void GhostPhysicsComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      // todo:
      // SerializeDataActor &actorData = Component::GetSerializeDataActor(dataContainer);
      // auto physCompData = SerializeHelper::GetSerializeDataPhysicsComponent(this);
      // physCompData->ComponentName = EngineObjectName;

      // actorData.ComponentsData.emplace_back(physCompData);
   }

   void GhostPhysicsComponent::SyncHitBoxScale(const glm::vec3& scale)
   {
      mDescriptor->GetShape()->GetCollisionShape()->setLocalScaling(Converter::glmToBullet(scale));
   }
}