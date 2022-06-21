#include "CombatEntity.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"

using namespace EngineCore;

namespace Game
{
    CombatEntity::CombatEntity(const std::shared_ptr<::EngineCore::Actor> &spaceShipActor,
                               const std::shared_ptr<::EngineCore::UiComponent> &uiComponent,
                               const size_t dmgTextFieldId)
        : mDamageDeltaTime(0.0f),
          mIsDamageReceived(false),
          mLifePoints(10),
          mSpaceShipActor(spaceShipActor),
          mUiComponent(uiComponent),
          mDmgTextFieldId(dmgTextFieldId)
    {
    }

    const std::shared_ptr<Actor> &CombatEntity::GetSpaceShipActor() const
    {
        return mSpaceShipActor;
    }

    bool CombatEntity::CheckIsAliveAfterDamage(const size_t dmg)
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

    void CombatEntity::RestoreLife()
    {
        mLifePoints = 10;
    }

    const std::shared_ptr<::EngineCore::UiComponent> &CombatEntity::GetSpaceShipUiComponent() const
    {
        return mUiComponent;
    }

    size_t CombatEntity::GetDmgTextFieldId() const
    {
        return mDmgTextFieldId;
    }
}