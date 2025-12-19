#include "MissileActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game {
MissileActor::MissileActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : Actor(gameObjectName, rootComponent)
    , mCombatActorsPoolHandler(combatActorsPoolHandler)
{
}

eMissileActivityState MissileActor::GetMissileActivityState() const
{
    return mActivityState;
}

void MissileActor::SetMissileActivityState(const eMissileActivityState activityState)
{
    mActivityState = activityState;
}

void MissileActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType damageDealerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    ext_assert(GetMovementComponent(), "MissileActor movement component is null");
    mDamageDealerType = damageDealerType;
    const auto& existingRotation = GetRootComponent()->GetAdditionalRotation();
    GetRootComponent()->SetAdditionalRotation(glm::vec3(existingRotation.x, yawDegrees, existingRotation.z));
    GetMovementComponent()->SetDirection(direction);
    GetMovementComponent()->Teleport(position);
}

void MissileActor::TriggerExplosion()
{
}

void MissileActor::TriggerExplosionFinished()
{
}

void MissileActor::TriggerDisabled()
{
}

std::shared_ptr<MissileExplosionVisitorBase> MissileActor::CreateMissileExplosionVisitor()
{
    return nullptr;
}

bool MissileActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
}

eMissileType MissileActor::GetMissileType() const
{
    return mMissileType;
}

eDamageDealerType MissileActor::GetDamageDealerType() const
{
    return mDamageDealerType;
}

std::shared_ptr<CombatActorsPoolHandler> MissileActor::GetCombatActorsPoolHandler() const
{
    return mCombatActorsPoolHandler;
}
} // namespace Game