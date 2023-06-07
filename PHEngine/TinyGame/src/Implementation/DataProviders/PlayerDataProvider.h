#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>

#include "Implementation/MissileType.h"

namespace Game
{
    class PlayerDataProvider
    {
        std::unordered_map<eMissileType, size_t> mAvailableMissiles;

        std::atomic<eMissileType> mSelectedMissileType{eMissileType::BOMB};

    public:
        static PlayerDataProvider *GetInstance();

        eMissileType GetSelectedMissileType() const;

        void SetSelectedMissileType(const eMissileType missileType);

        void SetMissilesCountForType(const eMissileType missileType, const size_t missilesCount);

        void SetMissilesCount(const std::unordered_map<eMissileType, size_t>& missilesMap);

        size_t GetMissilesCount(const eMissileType missileType) const;

        bool IsMissileAvailable(const eMissileType missileType) const;

        void SetAvailableMissileTypes(const std::unordered_map<eMissileType, size_t>& availableMissileTypes);

        std::vector<eMissileType> GetAvailableMissileTypes() const;
    };
}
