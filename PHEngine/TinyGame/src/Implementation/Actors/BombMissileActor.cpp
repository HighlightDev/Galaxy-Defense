#include "BombMissileActor.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/MissileExplosionVisitors/BombExplosionVisitor.h"

#include <algorithm>

namespace Game {
BombMissileActor::BombMissileActor(
    const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : MissileActor(gameObjectName, rootComponent)
{
    mMissileType = eMissileType::BOMB;
}

void BombMissileActor::TriggerSpawn(const glm::vec3& position, const eDamageDealerType ownerType)
{
    mDamageDealerType = ownerType;
    mActivityState = eMissileActivityState::ACTIVE;
    SetIsEnabled(true);
    GetMovementComponent()->Teleport(position);
}

void BombMissileActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    const auto c_soundList = GetComponentsByType<SoundComponent>();
    assert(c_soundList.size());
    const auto& soundSp = c_soundList.back();
    soundSp->GetSoundSource()->SetPitch(0.75f + (Random::Float() * 0.25f));
    soundSp->PlayBuffer("explosion");
    TriggerExplosionFinished();
}

void BombMissileActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
    TriggerDisabled();
}

void BombMissileActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;
    SetIsEnabled(false);
}

std::shared_ptr<MissileExplosionVisitorBase> BombMissileActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<BombExplosionVisitor>(std::static_pointer_cast<BombMissileActor>(shared_from_this()));
}
} // namespace Game