#pragma once

#include "Implementation/MissileType.h"

#include <atomic>
#include <unordered_map>
#include <vector>

namespace Game {
class PlayerDataProvider {
    std::unordered_map<eMissileType, size_t> mAvailableMissiles;

    std::atomic<eMissileType> mSelectedMissileType{eMissileType::BOMB};

    int32_t mDestroyedEnemySpaceshipsCount{0};

    int32_t mSelectedSpaceObjectId{-1};

    size_t mCrystalsCount{0};

private:
    explicit PlayerDataProvider();

public:
    static PlayerDataProvider* GetInstance();

    eMissileType GetSelectedMissileType() const;

    void SetSelectedMissileType(const eMissileType missileType);

    void SetMissilesCountForType(const eMissileType missileType, const size_t missilesCount);

    void SetMissilesCount(const std::unordered_map<eMissileType, size_t>& missilesMap);

    size_t GetMissilesCount(const eMissileType missileType) const;

    bool IsMissileAvailable(const eMissileType missileType) const;

    void SetAvailableMissileTypes(const std::unordered_map<eMissileType, size_t>& availableMissileTypes);

    std::vector<eMissileType> GetAvailableMissileTypes() const;

    void SetDestroyedEnemySpaceshipsCount(const int32_t enemySpaceshipsCount);

    int32_t GetDestroyedEnemySpaceshipsCount() const;

    void SetSelectedTower(const int32_t towerId, const eMissileType towerWeaponType);

    void SetSelectedBarrier(const int32_t barrierId);

    int32_t GetSelectedSpaceObjectId() const;

    size_t GetCrystalsCount() const;

    void SetCrystalsCount(const size_t crystalsCount);

    void ResetLevelData();
};
} // namespace Game
