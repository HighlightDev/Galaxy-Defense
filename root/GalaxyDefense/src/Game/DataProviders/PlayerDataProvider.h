#pragma once

#include "Game/MissileType.h"

#include <atomic>
#include <mutex>
#include <set>

namespace Game {
class PlayerDataProvider {
    std::set<eMissileType> mAvailableMissileTypes; // ordered: Lua shows towers in enum order

    int32_t mDestroyedEnemySpaceshipsCount{0};

    int32_t mSelectedSpaceObjectId{-1};

    std::atomic<int32_t> mCrystalsCount{0}; // written on the game thread, read from Lua thread

    mutable std::mutex mMissileTypesLock;

private:
    explicit PlayerDataProvider();

public:
    static PlayerDataProvider* GetInstance();

    void SetAvailableMissileTypes(std::set<eMissileType> availableMissileTypes);

    std::set<eMissileType> GetAvailableMissileTypes() const;

    void SetDestroyedEnemySpaceshipsCount(const int32_t enemySpaceshipsCount);

    int32_t GetDestroyedEnemySpaceshipsCount() const;

    void SetSelectedTower(const int32_t towerId, const eMissileType towerWeaponType);

    void SetSelectedBarrier(const int32_t barrierId);

    int32_t GetSelectedSpaceObjectId() const;

    int32_t GetCrystalsCount() const;

    void SetCrystalsCount(const int32_t crystalsCount);

    void ResetLevelData();
};
} // namespace Game
