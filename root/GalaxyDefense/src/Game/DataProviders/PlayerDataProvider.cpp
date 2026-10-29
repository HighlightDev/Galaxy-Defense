#include "PlayerDataProvider.h"

#include "Core/CommonCore/Assertion.h"
#include "Game/Constants/GameConstants.h"
#include "Game/Events/MainPlayerStatusChangedEvent.h"
#include "Game/StatusTypes.h"

#include <nlohmann/json.hpp>

#include <algorithm>

using namespace Event;

namespace Game {
PlayerDataProvider::PlayerDataProvider()
{
    mCrystalsCount = Game::Constants::c_crystalsCountFromStartup; // todo: should be reworked
}

PlayerDataProvider* PlayerDataProvider::GetInstance()
{
    static PlayerDataProvider s_dataProvider;
    return &s_dataProvider;
}

void PlayerDataProvider::SetAvailableMissileTypes(std::set<eMissileType> availableMissileTypes)
{
    {
        std::lock_guard<std::mutex> lock(mMissileTypesLock);
        mAvailableMissileTypes = std::move(availableMissileTypes);
    }
    const eMainPlayerStatusType playerStatusType = eMainPlayerStatusType::AVAILABLE_MISSILE_TYPES_CHANGED;
    nlohmann::json jsonObj;
    jsonObj["player_status_type"] = static_cast<int32_t>(playerStatusType);
    jsonObj["available_missile_types"] = mAvailableMissileTypes;
    jsonObj["missile_prices"] = Constants::c_missileTypePrices;
    const auto eventParams = jsonObj.dump();
    MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
    LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eventParams);
}

std::set<eMissileType> PlayerDataProvider::GetAvailableMissileTypes() const
{
    decltype(mAvailableMissileTypes) result;
    {
        std::lock_guard<std::mutex> lock(mMissileTypesLock);
        result = mAvailableMissileTypes;
    }
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

void PlayerDataProvider::SetCrystalsCount(const int32_t crystalsCount)
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

int32_t PlayerDataProvider::GetCrystalsCount() const
{
    return mCrystalsCount;
}

void PlayerDataProvider::ResetLevelData()
{
    {
        std::lock_guard<std::mutex> lock(mMissileTypesLock);
        mAvailableMissileTypes.clear();
    }
    mDestroyedEnemySpaceshipsCount = 0;
    mSelectedSpaceObjectId = -1;
}
} // namespace Game
