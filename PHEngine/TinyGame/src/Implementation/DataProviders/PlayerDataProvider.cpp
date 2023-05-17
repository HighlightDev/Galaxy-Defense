#include "PlayerDataProvider.h"
#include "Core/CommonCore/Assertion.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/StatusTypes.h"

using namespace Event;

namespace Game
{
    PlayerDataProvider *PlayerDataProvider::GetInstance()
    {
        static PlayerDataProvider s_dataProvider;
        return &s_dataProvider;
    }

    eMissileType PlayerDataProvider::GetSelectedMissileType() const
    {
        return mSelectedMissileType.load(std::memory_order::memory_order_seq_cst);
    }

    void PlayerDataProvider::SetSelectedMissileType(const eMissileType missileType)
    {
        mSelectedMissileType.store(missileType, std::memory_order::memory_order_seq_cst);
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::ACTIVE_WEAPON_CHANGED);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::ACTIVE_WEAPON_CHANGED);
    }

    void PlayerDataProvider::SetMissilesCount(const eMissileType missileType, const size_t missilesCount)
    {
        assert(mAvailableMissiles.count(missileType));
        mAvailableMissiles[missileType] = missilesCount;
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
    }

    size_t PlayerDataProvider::GetMissilesCount(const eMissileType missileType) const
    {
        assert(mAvailableMissiles.count(missileType));
        return mAvailableMissiles.at(missileType);
    }

    bool PlayerDataProvider::IsMissileAvailable(const eMissileType missileType) const
    {
        return mAvailableMissiles.count(missileType) > 0;
    }
}
