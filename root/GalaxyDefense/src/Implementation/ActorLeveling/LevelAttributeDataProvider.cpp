#include "LevelAttributeDataProvider.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/ActorLeveling/SpaceStationLevel.h"

#include <unordered_map>
#include <vector>

namespace Game {

int32_t LevelAttributeDataProvider::GetCooldownForMissileTypeAtLevel(const eMissileType missileType, const int32_t level)
{
    std::unordered_map<eMissileType, std::vector<int32_t>> cooldownData = {
        {eMissileType::BOMB, {1000, 900, 800, 700, 600}},
        {eMissileType::FREEZING_BOMB, {1200, 1100, 1000, 900, 800}},
        {eMissileType::ELECTRO_RAY, {3000, 2900, 2800, 2700, 2400}},
        {eMissileType::BLACK_HOLE, {6500, 4500, 4000, 3500, 3000}},
        {eMissileType::PLASMA_BOMB, {4000, 3600, 3200, 2900, 2600}},
        {eMissileType::FREEZING_RAY, {1500, 1400, 1300, 1200, 1100}},
        {eMissileType::REPAIR_BEAM, {1500, 1400, 1300, 1200, 1100}},
        {eMissileType::FORCE_BARRIER, {2000, 1900, 1800, 1700, 1600}},
    };

    if (cooldownData.find(missileType) != cooldownData.end()) {
        const auto& cooldowns = cooldownData.at(missileType);
        if (level >= 1 && level <= static_cast<int32_t>(cooldowns.size())) {
            return cooldowns.at(level - 1);
        }
    }

    ext_assert(false, "Invalid missile type or level");
    return -1;
}

float LevelAttributeDataProvider::GetRadiusForMissileTypeAtLevel(const eMissileType missileType, const int32_t level)
{
    std::unordered_map<eMissileType, std::vector<float>> radiusesData = {
        {eMissileType::BOMB, {30.0, 40.0, 50.0, 55.0, 60.0}},
        {eMissileType::FREEZING_BOMB, {30.0, 40.0, 50.0, 55.0, 60.0}},
        {eMissileType::ELECTRO_RAY, {30.0, 35.0, 40.0, 45.0, 50.0}},
        {eMissileType::BLACK_HOLE, {50.0, 55.0, 55.0, 60.0, 60.0}},
        {eMissileType::PLASMA_BOMB, {35.0, 40.0, 45.0, 50.0, 55.0}},
        {eMissileType::FREEZING_RAY, {50.0, 60.0, 70.0, 75.0, 80.0}},
        {eMissileType::REPAIR_BEAM, {40.0, 50.0, 60.0, 65.0, 70.0}},
        {eMissileType::FORCE_BARRIER, {40.0, 50.0, 60.0, 65.0, 70.0}},
    };

    if (radiusesData.contains(missileType)) {
        const auto& radiuses = radiusesData.at(missileType);
        if (level >= 1 && level <= static_cast<int32_t>(radiuses.size())) {
            return radiuses.at(level - 1);
        }
    }

    ext_assert(false, "Invalid missile type or level");
    return -1;
}

} // namespace Game