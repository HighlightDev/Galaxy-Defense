#include "LevelSerializationHelper.h"

#include "Core/CommonCore/Assertion.h"

#include <json/json.hpp>

#include <algorithm>

namespace Game {
std::string LevelSerializationHelper::DumpLevelToJsonString(const LevelData& levelData) const
{
    auto preparedTowersData = PrepareTowersData(levelData.TowersData);
    auto preparedBarriersData = PrepareBarriersData(levelData.BarriersData);

    nlohmann::json jsonObj;
    jsonObj["level_name"] = levelData.LevelName;
    jsonObj["level_boundary_min"] = JsonVec2(levelData.LevelBoundaryMin);
    jsonObj["level_boundary_max"] = JsonVec2(levelData.LevelBoundaryMax);
    jsonObj["towers"] = preparedTowersData;
    jsonObj["barriers"] = preparedBarriersData;
    ext_assert(levelData.DestinationPoint.has_value(), "Level data must contain destination point");
    jsonObj["destination_point"] = JsonVec3(levelData.DestinationPoint.value());
    jsonObj["spawn_portals"] = PrepareSpawnPortalsData(levelData.SpawnPortalsData);

    return jsonObj.dump();
}

LevelData LevelSerializationHelper::RestoreLevelFromJsonString(const std::string& jsonStr) const
{
    const auto& jsonObj = nlohmann::json::parse(jsonStr);
    LevelData lvlData;
    lvlData.LevelName = jsonObj.at("level_name").get<std::string>();
    const auto& min = jsonObj.at("level_boundary_min").get<JsonVec2>();
    const auto& max = jsonObj.at("level_boundary_max").get<JsonVec2>();
    lvlData.LevelBoundaryMin = glm::vec2(min.x, min.y);
    lvlData.LevelBoundaryMax = glm::vec2(max.x, max.y);
    auto preparedTowersData = jsonObj.at("towers").get<std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>>>();
    lvlData.TowersData = RestoreTowers(preparedTowersData);
    auto preparedBarriersData = jsonObj.at("barriers").get<std::unordered_map<std::string, std::vector<JsonVec3>>>();
    lvlData.BarriersData = RestoreBarriers(preparedBarriersData);

    ext_assert(jsonObj.contains("destination_point"), "Level data json must contain destination point data");
    const auto& dp = jsonObj.at("destination_point").get<JsonVec3>();
    lvlData.DestinationPoint = glm::vec3(dp.x, dp.y, dp.z);

    if (jsonObj.contains("spawn_portals")) {
        auto preparedSpawnPortalsData = jsonObj.at("spawn_portals").get<std::vector<JsonVec3>>();
        lvlData.SpawnPortalsData = RestoreSpawnPortals(preparedSpawnPortalsData);
    }

    return lvlData;
}

std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>> LevelSerializationHelper::PrepareTowersData(
    const std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>>& towersData) const
{
    std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>> result;
    result.reserve(towersData.size());

    std::transform(towersData.cbegin(), towersData.cend(), std::inserter(result, result.begin()), [](const auto& towerPair) {
        const auto &position = std::get<0>(towerPair.second), scale = std::get<1>(towerPair.second);
        return std::make_pair(towerPair.first, std::make_tuple(position, scale));
    });

    return result;
}

std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>>
LevelSerializationHelper::RestoreTowers(const std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>>& towersData) const
{
    std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> result;
    result.reserve(towersData.size());

    std::transform(towersData.cbegin(), towersData.cend(), std::inserter(result, result.begin()), [](const auto& towerPair) {
        const auto &first = std::get<0>(towerPair.second), second = std::get<1>(towerPair.second);

        const glm::vec3& position = glm::vec3(first.x, first.y, first.z);
        const glm::vec3& scale = glm::vec3(second.x, second.y, second.z);
        return std::make_pair(towerPair.first, std::make_tuple(position, scale));
    });

    return result;
}

std::unordered_map<std::string, std::vector<JsonVec3>>
LevelSerializationHelper::PrepareBarriersData(const std::unordered_map<std::string, std::vector<glm::vec3>>& barrierssData) const
{
    std::unordered_map<std::string, std::vector<JsonVec3>> result;
    result.reserve(barrierssData.size());

    for (const auto& [barrierName, pillarsPoints] : barrierssData) {
        result[barrierName] = {};
        std::transform(
            pillarsPoints.cbegin(),
            pillarsPoints.cend(),
            std::inserter(result[barrierName], result[barrierName].begin()),
            [](const auto& pillarPoint) { return pillarPoint; });
    }

    return result;
}

std::unordered_map<std::string, std::vector<glm::vec3>>
LevelSerializationHelper::RestoreBarriers(const std::unordered_map<std::string, std::vector<JsonVec3>>& barriersData) const
{
    std::unordered_map<std::string, std::vector<glm::vec3>> result;
    result.reserve(barriersData.size());
    for (const auto& [barrierName, pillarsPoints] : barriersData) {
        result[barrierName] = {};
        std::transform(
            pillarsPoints.cbegin(),
            pillarsPoints.cend(),
            std::inserter(result[barrierName], result[barrierName].begin()),
            [](const auto& parsedBarrierPillarPoint) {
                return glm::vec3(parsedBarrierPillarPoint.x, parsedBarrierPillarPoint.y, parsedBarrierPillarPoint.z);
            });
    }

    return result;
}
std::vector<JsonVec3> LevelSerializationHelper::PrepareSpawnPortalsData(const std::vector<glm::vec3>& spawnPortalsData) const
{
    std::vector<JsonVec3> result;
    result.reserve(spawnPortalsData.size());
    std::transform(spawnPortalsData.cbegin(), spawnPortalsData.cend(), std::back_inserter(result), [](const auto& pos) {
        return JsonVec3(pos);
    });
    return result;
}

std::vector<glm::vec3> LevelSerializationHelper::RestoreSpawnPortals(const std::vector<JsonVec3>& spawnPortalsData) const
{
    std::vector<glm::vec3> result;
    result.reserve(spawnPortalsData.size());
    std::transform(spawnPortalsData.cbegin(), spawnPortalsData.cend(), std::back_inserter(result), [](const auto& p) {
        return glm::vec3(p.x, p.y, p.z);
    });
    return result;
}
} // namespace Game
