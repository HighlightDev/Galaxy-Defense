#include "FreezingMissileActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.h"

namespace Game {
FreezingMissileActor::FreezingMissileActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& poolHandler)
    : MissileActor(gameObjectName, rootComponent, poolHandler)
{
    mMissileType = eMissileType::FREEZING_BOMB;
}

void FreezingMissileActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<Actor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mActivityState = eMissileActivityState::ACTIVE;
    SetIsEnabled(true);
    const auto& existingRotation = GetRootComponent()->GetAdditionalRotation();
    GetRootComponent()->SetAdditionalRotation(glm::vec3(existingRotation.x, yawDegrees, existingRotation.z));
    GetMovementComponent()->SetDirection(direction);
    GetMovementComponent()->Teleport(position);
}

void FreezingMissileActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    const auto c_soundList = GetComponentsByType<SoundComponent>();
    assert(c_soundList.size());
    c_soundList.back()->PlayBuffer("explosion");

    const auto c_particle = GetComponentsByType<ParticleSystemComponent>().back();
    c_particle->EmitParticles();

    const auto c_mesh = GetComponentsByType<StaticMeshComponent>().back();
    c_mesh->SetIsEnabled(false);

    const auto& c_physics = GetPhysicsComponent();
    c_physics->SetIsEnabled(false);

    const auto& movement = GetMovementComponent();
    movement->SetIsEnabled(false);
}

void FreezingMissileActor::Tick(const float deltaTime)
{
    MissileActor::Tick(deltaTime);

    if (mActivityState == eMissileActivityState::EXPLOSION) {
        explosionTime += deltaTime;
        if (explosionTime >= 2.0f) {
            explosionTime = 0.0f;
            TriggerExplosionFinished();
        }
    }
}

void FreezingMissileActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
    TriggerDisabled();
}

void FreezingMissileActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;

    const auto c_particle = GetComponentsByType<ParticleSystemComponent>().back();
    c_particle->ResetParticles();

    SetIsEnabled(false);
}

std::shared_ptr<MissileExplosionVisitorBase> FreezingMissileActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<FreezingExplosionVisitor>(std::static_pointer_cast<FreezingMissileActor>(shared_from_this()));
}
} // namespace Game