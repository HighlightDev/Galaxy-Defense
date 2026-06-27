#include "ElectroRayExplosionVisitor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Events/ElectroRaySphereContactCollisionEvent.h"

#include <algorithm>

using namespace EngineCore;

#undef min
#undef max

namespace Game {
ElectroRayExplosionVisitor::ElectroRayExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void ElectroRayExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            const uint32_t dmg = static_cast<uint32_t>(Random::Float() * 3.0f) + 7;
            spaceship->TriggerDamageReceived(dmg, ownerSp->GetDamageDealerType());
            if (!spaceship->IsAlive()) {
                ownerSp->TriggerDisabled();
            }

            if (const auto& sceneSp = ownerSp->GetSceneOwner().lock()) {
                SphereCollisionTestWithFilterAdapter collisionTest(50.0f, {spaceship->GetPhysicsComponent()});
                collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), spaceship->GetRootComponent()->GetTranslation());
                const auto& collidedDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
                std::vector<int32_t> descriptorActorIds;
                std::transform(
                    collidedDescriptors.begin(),
                    collidedDescriptors.end(),
                    std::back_inserter(descriptorActorIds),
                    [](const auto& collidedDescriptor) { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

                Event::ElectroRaySphereContactCollisionEvent::GetInstance()->SendEvent(
                    eExecutionOrder::PRE_EXECUTION, spaceship->GetObjectId(), descriptorActorIds);
            }
        }
    }
}

void ElectroRayExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void ElectroRayExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{

    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            spaceObject->TriggerDisabled();
            ownerSp->TriggerDisabled();

            if (const auto& sceneSp = ownerSp->GetSceneOwner().lock()) {
                SphereCollisionTestWithFilterAdapter collisionTest(50.0f, {spaceObject->GetPhysicsComponent()});
                collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), spaceObject->GetRootComponent()->GetTranslation());
                const auto& collidedDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
                std::vector<int32_t> descriptorActorIds;
                std::transform(
                    collidedDescriptors.begin(),
                    collidedDescriptors.end(),
                    std::back_inserter(descriptorActorIds),
                    [](const auto& collidedDescriptor) { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

                Event::ElectroRaySphereContactCollisionEvent::GetInstance()->SendEvent(
                    eExecutionOrder::PRE_EXECUTION, spaceObject->GetObjectId(), descriptorActorIds);
            }
        }
    }
}

void ElectroRayExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void ElectroRayExplosionVisitor::StartExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void ElectroRayExplosionVisitor::EndExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}
} // namespace Game