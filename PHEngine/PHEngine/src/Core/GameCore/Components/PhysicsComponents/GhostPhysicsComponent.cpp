#include "GhostPhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

using namespace EngineMath;

namespace EnginePhysics
{
   GhostPhysicsComponent::GhostPhysicsComponent(const PhysicsComponentData &data)
       : PhysicsComponent(data)
   {
   }

   GhostPhysicsComponent::~GhostPhysicsComponent()
   {
   }

   void GhostPhysicsComponent::Tick(const float deltaTime)
   {
      Component::Tick(deltaTime);

      if (const auto &spOwner = GetOwner().lock())
      {
         const glm::vec3 &translation = spOwner->GetRootComponent()->GetTranslation();
         const glm::quat &rotator = spOwner->GetRootComponent()->GetRotator();
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
}