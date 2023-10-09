#include "MissileActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Implementation/MissileExplosionVisitors/BombExplosionVisitor.h"

namespace Game
{
    MissileActor::MissileActor(const std::string &gameObjectName,
                               const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent)
    {
    }

    eMissileActivityState MissileActor::GetMissileActivityState() const
    {
        return mActivityState;
    }

    void MissileActor::TriggerSpawn(const glm::vec3& position, const eDamageDealerType damageDealerType)
    {
        assert(GetMovementComponent());
        mDamageDealerType = damageDealerType;
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

    bool MissileActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
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
}