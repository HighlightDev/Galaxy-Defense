#include "BombMissileActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Implementation/MissileExplosionVisitors/BombExplosionVisitor.h"

namespace Game
{
    BombMissileActor::BombMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : MissileActor(gameObjectName, rootComponent)
    {
    }

    void BombMissileActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eMissileActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void BombMissileActor::TriggerExplosion()
    {
        mActivityState = eMissileActivityState::EXPLOSION;
        const auto c_soundList = GetComponentsByType<SoundComponent>();
        assert(c_soundList.size());
        c_soundList.back()->PlayBuffer("explosion");
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
}