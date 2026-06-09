#include "PlayerDataProvider.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/StatusTypes.h"

#include <json/json.hpp>

#include <algorithm>

using namespace Event;

namespace Game {
PlayerDataProvider::PlayerDataProvider()
{
#if defined(DEBUG)
    LogInfo("PlayerDataProvider::Give a lot of crystals from the start for debug purposes");
    mCrystalsCount = 9999;
#endif
}

PlayerDataProvider* PlayerDataProvider::GetInstance()
{
    static PlayerDataProvider s_dataProvider;
    return &s_dataProvider;
}

eMissileType PlayerDataProvider::GetSelectedMissileType() const
{
    return mSelectedMissileType.load(std::memory_order::seq_cst);
}

void PlayerDataProvider::SetSelectedMissileType(const eMissileType missileType)
{
    mSelectedMissileType.store(missileType, std::memory_order::seq_cst);
}

void PlayerDataProvider::SetMissilesCountForType(const eMissileType missileType, const size_t missilesCount)
{
    ext_assert(mAvailableMissiles.count(missileType), "Unknown missile type in SetMissilesCountForType");
    if (mAvailableMissiles.at(missileType) != missilesCount) {
        mAvailableMissiles[missileType] = missilesCount;
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
}

size_t PlayerDataProvider::GetMissilesCount(const eMissileType missileType) const
{
    ext_assert(mAvailableMissiles.count(missileType), "Unknown missile type in GetMissilesCount");
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
    LogInfo("PlayerDataProvider::SetDestroyedEnemySpaceshipsCount: enemySpaceshipsCount: ", enemySpaceshipsCount);
    mDestroyedEnemySpaceshipsCount = enemySpaceshipsCount;
}

int32_t PlayerDataProvider::GetDestroyedEnemySpaceshipsCount() const
{
    return mDestroyedEnemySpaceshipsCount;
}

void PlayerDataProvider::SetSelectedTower(const int32_t towerId, const eMissileType towerWeaponType)
{
    LogInfo(
        "PlayerDataProvider::SetSelectedTower: towerId: ", towerId, ", towerWeaponType: ", static_cast<int32_t>(towerWeaponType));
    if (mSelectedSpaceObjectId != towerId) {
        mSelectedSpaceObjectId = towerId;
        const eMainPlayerStatusType playerStatusType = eMainPlayerStatusType::SELECTED_SPACE_OBJECT_CHANGED;
        nlohmann::json jsonObj;
        jsonObj["player_status_type"] = static_cast<int32_t>(playerStatusType);
        jsonObj["has_selected_tower"] = towerId != -1;
        jsonObj["tower_weapon_type"] = static_cast<int32_t>(towerWeaponType);
        const auto& eventParams = jsonObj.dump();
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
    }
}

void PlayerDataProvider::SetSelectedBarrier(const int32_t barrierId)
{
    LogInfo("PlayerDataProvider::SetSelectedBarrier: barrierId: ", barrierId);
    if (mSelectedSpaceObjectId != barrierId) {
        mSelectedSpaceObjectId = barrierId;
        const eMainPlayerStatusType playerStatusType = eMainPlayerStatusType::SELECTED_SPACE_OBJECT_CHANGED;
        nlohmann::json jsonObj;
        jsonObj["player_status_type"] = static_cast<int32_t>(playerStatusType);
        jsonObj["has_selected_barrier"] = barrierId != -1;
        const auto& eventParams = jsonObj.dump();
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
    }
}

int32_t PlayerDataProvider::GetSelectedSpaceObjectId() const
{
    return mSelectedSpaceObjectId;
}

void PlayerDataProvider::SetCrystalsCount(const size_t crystalsCount)
{
    LogInfo("PlayerDataProvider::SetCrystalsCount: crystalsCount: ", crystalsCount);
    if (mCrystalsCount != crystalsCount) {
        mCrystalsCount = crystalsCount;
        const eMainPlayerStatusType playerStatusType = eMainPlayerStatusType::CRYSTALS_COUNT_CHANGED;
        nlohmann::json jsonObj;
        jsonObj["player_status_type"] = static_cast<int32_t>(playerStatusType);
        jsonObj["crystals_count"] = crystalsCount;
        const auto& eventParams = jsonObj.dump();
        MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
        LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
    }
}

size_t PlayerDataProvider::GetCrystalsCount() const
{
    return mCrystalsCount;
}

void PlayerDataProvider::ResetLevelData()
{
    mAvailableMissiles.clear();
    mSelectedMissileType = eMissileType::NONE;
    mDestroyedEnemySpaceshipsCount = 0;
    mSelectedSpaceObjectId = -1;
    mCrystalsCount = 0;
}
} // namespace Game
