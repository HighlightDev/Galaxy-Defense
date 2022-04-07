#include "CombatEntity.h"
#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game
{
    CombatEntity::CombatEntity(const std::shared_ptr<Actor> &spaceShipActor)
        : mDamageDeltaTime(0.0f),
          mIsDamageReceived(false),
          mLifePoints(2),
          mSpaceShipActor(spaceShipActor)
    {
    }

    const std::shared_ptr<Actor> &CombatEntity::GetSpaceShipActor() const
    {
        return mSpaceShipActor;
    }

    bool CombatEntity::CheckIsAliveAfterDamage(const float dmg)
    {
        mLifePoints = mLifePoints >= dmg ? mLifePoints - dmg : 0;
        return 0 != mLifePoints;
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