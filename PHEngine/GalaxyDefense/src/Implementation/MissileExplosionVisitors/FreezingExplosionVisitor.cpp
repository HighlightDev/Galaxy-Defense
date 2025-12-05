#include "FreezingExplosionVisitor.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/Modifiers/FreezingModifier.h"

using namespace EngineCore;

namespace Game {
FreezingExplosionVisitor::FreezingExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void FreezingExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            AddOrResetFreezingModifier(spaceship);
            ProcessProjectileWithSpaceshipsCollision(ownerSp->GetRootComponent()->GetTranslation());
            ownerSp->TriggerExplosion();
        }
    }
}

void FreezingExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void FreezingExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            ownerSp->TriggerExplosion();
            spaceObject->TriggerDisabled();
        }
    }
}

void FreezingExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void FreezingExplosionVisitor::ProcessProjectileWithSpaceshipsCollision(const glm::vec3& explosionCenterPosition)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (const auto& sceneSp = ownerSp->GetSceneOwner().lock()) {
            const auto& combatActorsPoolHandlerSp = ownerSp->GetCombatActorsPoolHandler();

            const auto& spaceStations = combatActorsPoolHandlerSp->GetSpaceStationActors();
            std::vector<std::shared_ptr<PhysicsComponent>> excludedPhysicsComponents;
            const auto& spaceStationsPhysComponents = combatActorsPoolHandlerSp->GetSpaceStationsPhysicsComponents();
            const auto& bombMissilePhysComponents = combatActorsPoolHandlerSp->GetMissilePhysicsComponents(eMissileType::BOMB);
            const auto& freezeMissilePhysComponents
                = combatActorsPoolHandlerSp->GetMissilePhysicsComponents(eMissileType::FREEZING_BOMB);
            const auto& blackHoleMissilePhysComponents
                = combatActorsPoolHandlerSp->GetMissilePhysicsComponents(eMissileType::BLACK_HOLE);
            excludedPhysicsComponents.reserve(spaceStations.size() + combatActorsPoolHandlerSp->GetMissileActors().size());
            excludedPhysicsComponents.insert(
                excludedPhysicsComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
            excludedPhysicsComponents.insert(
                excludedPhysicsComponents.end(), bombMissilePhysComponents.begin(), bombMissilePhysComponents.end());
            excludedPhysicsComponents.insert(
                excludedPhysicsComponents.end(), freezeMissilePhysComponents.begin(), freezeMissilePhysComponents.end());
            excludedPhysicsComponents.insert(
                excludedPhysicsComponents.end(), blackHoleMissilePhysComponents.begin(), blackHoleMissilePhysComponents.end());

            constexpr float c_collisionSphereRadius = 15.0f;
            SphereCollisionTestWithFilterAdapter collisionTest(c_collisionSphereRadius, excludedPhysicsComponents);
            collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), explosionCenterPosition);
            const auto& collidedDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
            std::vector<int32_t> descriptorActorIds;
            std::transform(
                collidedDescriptors.begin(),
                collidedDescriptors.end(),
                std::back_inserter(descriptorActorIds),
                [](const auto& collidedDescriptor) { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

            for (const auto& descriptorActorId : descriptorActorIds) {
                const auto& shipSp = combatActorsPoolHandlerSp->GetEnemyShipOwnerActorById(descriptorActorId);
                ext_assert(shipSp, "FreezingExplosionVisitor enemy ship pointer is null");
                AddOrResetFreezingModifier(shipSp);
            }
        }
    }
}

void FreezingExplosionVisitor::AddOrResetFreezingModifier(const std::shared_ptr<SpaceshipActor>& spaceship)
{
    if (!spaceship->HasModifier(eModifierType::Freezing)) {
        const auto freezingModifier = std::make_shared<FreezingModifier>(spaceship);
        freezingModifier->SetFreezingPower(5.0f);
        spaceship->AddModifier(freezingModifier);
    } else {
        const auto freezingModifier = std::static_pointer_cast<FreezingModifier>(spaceship->GetModifier(eModifierType::Freezing));
        freezingModifier->ResetFreezingTimer();
    }
}
} // namespace Game