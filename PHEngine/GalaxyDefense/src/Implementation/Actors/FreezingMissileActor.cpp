#include "FreezingMissileActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.h"

namespace Game
{
    FreezingMissileActor::FreezingMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : MissileActor(gameObjectName, rootComponent)
    {
        mMissileType = eMissileType::FREEZING;
    }

    void FreezingMissileActor::TriggerSpawn(const glm::vec3 &position, const eDamageDealerType ownerType)
    {
        mDamageDealerType = ownerType;
        mActivityState = eMissileActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void FreezingMissileActor::TriggerExplosion()
    {
        mActivityState = eMissileActivityState::EXPLOSION;
        const auto c_soundList = GetComponentsByType<SoundComponent>();
        assert(c_soundList.size());
        c_soundList.back()->PlayBuffer("explosion");
        TriggerExplosionFinished();
    }

    void FreezingMissileActor::TriggerExplosionFinished()
    {
        mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
        TriggerDisabled();
    }

    void FreezingMissileActor::TriggerDisabled()
    {
        mActivityState = eMissileActivityState::IDLE;
        SetIsEnabled(false);
    }

    std::shared_ptr<MissileExplosionVisitorBase> FreezingMissileActor::CreateMissileExplosionVisitor()
    {
        return std::make_shared<FreezingExplosionVisitor>(std::static_pointer_cast<FreezingMissileActor>(shared_from_this()));
    }
}