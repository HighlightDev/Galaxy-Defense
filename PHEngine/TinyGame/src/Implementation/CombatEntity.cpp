#include "CombatEntity.h"
#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game
{
    CombatEntity::CombatEntity(const std::shared_ptr<Actor> &spaceShipActor)
        : mDamageDeltaTime(0.0f),
          mIsDamageReceived(false),
          mSpaceShipActor(spaceShipActor)
    {
    }

    const std::shared_ptr<Actor> &CombatEntity::GetSpaceShipActor() const
    {
        return mSpaceShipActor;
    }

    void CombatEntity::SetDamageDeltaTime(const float deltaTime)
    {
        mDamageDeltaTime = deltaTime;
    }

    float CombatEntity::GetDamageDeltaTime() const
    {
        return mDamageDeltaTime;
    }

    void CombatEntity::SetIsDamageReceived(const bool isDamageReceived)
    {
        mIsDamageReceived = isDamageReceived;
    }

    bool CombatEntity::GetIsDamageReceived() const
    {
        return mIsDamageReceived;
    }
}