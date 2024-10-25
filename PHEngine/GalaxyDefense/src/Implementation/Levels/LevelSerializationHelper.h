#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <tuple>
#include <unordered_map>

#include "Core/CommonCore/JsonHelper.h"
#include "LevelData.h"

namespace Game
{
    struct LevelSerializationHelper
    {
        std::string DumpLevelToJsonString(const LevelData &levelData) const;

        LevelData RestoreLevelFromJsonString(const std::string &jsonStr) const;

    private:
        std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>>
        PrepareRouteControlPointsData(const std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> &routeControlPoints) const;

        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>>
        RestoreRouteControlPoints(const std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>> &routeControlPoints) const;

        std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>>
        PrepareTowersData(const std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> &towersData) const;

        std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>>
        RestoreTowers(const std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>> &towersData) const;

        std::unordered_map<std::string, std::vector<JsonVec3>>
        PrepareBarriersData(const std::unordered_map<std::string, std::vector<glm::vec3>> &barrierssData) const;

        std::unordered_map<std::string, std::vector<glm::vec3>>
        RestoreBarriers(const std::unordered_map<std::string, std::vector<JsonVec3>> &barriersData) const;
    };
}
