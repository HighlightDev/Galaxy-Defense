#pragma once

#include <atomic>
#include <unordered_map>

#include "Implementation/MissileType.h"

namespace Game
{
    class PlayerDataProvider
    {
        std::unordered_map<eMissileType, size_t> mAvailableMissiles;

        std::atomic<eMissileType> mSelectedMissileType{eMissileType::NONE};

    public:
        static PlayerDataProvider *GetInstance();

        eMissileType GetSelectedMissileType() const;

        void SetSelectedMissileType(const eMissileType missileType);

        void SetMissilesCount(const eMissileType missileType, const size_t missilesCount);

        size_t GetMissilesCount(const eMissileType missileType) const;

        bool IsMissileAvailable(const eMissileType missileType) const;
    };
}
