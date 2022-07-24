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
         m_CollisionShapeScale(std::make_shared<EngineGOProperty<glm::vec3>>(glm::vec3(1.0f),
                                                                             "p_collisionShapeScale",
                                                                             [this](const glm::vec3 &value)
                                                                             { SyncCollisionShapeScale(value); }))
   {
      AddEngineProperty(m_CollisionShapeScale);
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
      // SerializeDataActor &actorData = Component::GetSerializeDataActor(dataContainer);
      // auto physCompData = SerializeHelper::GetSerializeDataPhysicsComponent(this);
      // physCompData->ComponentName = GameObjectName;

      // actorData.ComponentsData.emplace_back(physCompData);
   }

   void GhostPhysicsComponent::SyncCollisionShapeScale(const glm::vec3& scale)
   {
      mDescriptor->GetShape()->GetCollisionShape()->setLocalScaling(Converter::glmToBullet(scale));
   }
}