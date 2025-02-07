#include "PlayerDataProvider.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/StatusTypes.h"

#include <algorithm>

using namespace Event;

namespace Game {
PlayerDataProvider* PlayerDataProvider::GetInstance()
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
    MainPlayerStatusChangedEvent::GetInstance()->SendEvent(
        eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::ACTIVE_WEAPON_CHANGED);
    LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(
        eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::ACTIVE_WEAPON_CHANGED);
}

void PlayerDataProvider::SetMissilesCountForType(const eMissileType missileType, const size_t missilesCount)
{
    assert(mAvailableMissiles.count(missileType));
    if (mAvailableMissiles.at(missileType) != missilesCount) {
        mAvailableMissiles[missileType] = missilesCount;
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(
            eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(
            eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
    }
}

void PlayerDataProvider::SetMissilesCount(const std::unordered_map<eMissileType, size_t>& missilesMap)
{
    bool bDataUpdated = false;
    for (const auto [missileType, actualCount] : mAvailableMissiles) {
        const auto newCount = missilesMap.at(missileType);
        if (newCount != actualCount) {
            mAvailableMissiles[missileType] = newCount;
            bDataUpdated = true;
        }
    }
    if (bDataUpdated) {
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(
            eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(
            eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::MISSILES_COUNT_CHANGED);
    }
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

void PlayerDataProvider::SetAvailableMissileTypes(const std::unordered_map<eMissileType, size_t>& availableMissileTypes)
{
    mAvailableMissiles = availableMissileTypes;
}

std::vector<eMissileType> PlayerDataProvider::GetAvailableMissileTypes() const
{
    std::vector<eMissileType> result;
    std::transform(
        mAvailableMissiles.cbegin(), mAvailableMissiles.cend(), std::back_inserter(result), [](const auto& availableMissilePair) {
            return availableMissilePair.first;
        });
    return result;
}

void PlayerDataProvider::SetDestroyedEnemySpaceshipsCount(const int32_t enemySpaceshipsCount)
{
    LogInfo("PlayerDataProvider::SetDestroyedEnemySpaceshipsCount => enemySpaceshipsCount: ", enemySpaceshipsCount);
    mDestroyedEnemySpaceshipsCount = enemySpaceshipsCount;
    MainPlayerStatusChangedEvent::GetInstance()->SendEvent(
        eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED);
    LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(
        eExecutionOrder::PRE_EXECUTION, eMainPlayerStatusType::DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED);
}

int32_t PlayerDataProvider::GetDestroyedEnemySpaceshipsCount() const
{
    return mDestroyedEnemySpaceshipsCount;
}
} // namespace Game
