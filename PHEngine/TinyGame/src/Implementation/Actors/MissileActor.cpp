#include "MissileActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Implementation/MissileExplosionVisitors/BombExplosionVisitor.h"

namespace Game
{
    MissileActor::MissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent)
    {
    }

    eMissileActivityState MissileActor::GetMissileActivityState() const
    {
        return mActivityState;
    }

    bool MissileActor::IsInsideLevel(const BoundingBox &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
    }

    void MissileActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eMissileActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void MissileActor::TriggerExplosion()
    {
        mActivityState = eMissileActivityState::EXPLOSION;
        const auto c_soundList = GetComponentsByType<SoundComponent>();
        assert(c_soundList.size());
        c_soundList.back()->PlayBuffer("explosion");
        TriggerDisable();
    }

    void MissileActor::TriggerDisable()
    {
        mActivityState = eMissileActivityState::IDLE;
        SetIsEnabled(false);
    }

    std::shared_ptr<MissileExplosionVisitorBase> MissileActor::CreateMissileExplosionVisitor()
    {
        return std::make_shared<BombExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
    }
}