#include "LevelSerializationHelper.h"

#include <algorithm>
#include <json/json.hpp>

namespace Game
{
    std::string LevelSerializationHelper::DumpLevelToJsonString(const LevelData &levelData) const
    {
        auto preparedRoutesData = PrepareRouteControlPointsData(levelData.RoutesData);
        auto preparedTowersData = PrepareTowersData(levelData.TowersData);

        nlohmann::json jsonObj;
        jsonObj["level_name"] = levelData.LevelName;
        jsonObj["routes"] = preparedRoutesData;
        jsonObj["towers"] = preparedTowersData;

        return jsonObj.dump();
    }

    LevelData LevelSerializationHelper::RestoreLevelFromJsonString(const std::string &jsonStr) const
    {
        const auto &jsonObj = nlohmann::json::parse(jsonStr);
        LevelData lvlData;
        lvlData.LevelName = jsonObj.at("level_name").get<std::string>();
        auto preparedRoutesData = jsonObj.at("routes").get<std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>>>();
        lvlData.RoutesData = RestoreRouteControlPoints(preparedRoutesData);
        auto preparedTowersData = jsonObj.at("towers").get<std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>>>();
        lvlData.TowersData = RestoreTowers(preparedTowersData);
        return lvlData;
    }

    std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>>
    LevelSerializationHelper::PrepareRouteControlPointsData(const std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> &routeControlPoints) const
    {
        std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>> result;
        result.reserve(routeControlPoints.size());

        for (const auto &[routeName, route] : routeControlPoints)
        {
            result[routeName] = {};
            std::transform(route.cbegin(), route.cend(),
                           std::inserter(result[routeName], result[routeName].begin()),
                           [](const auto &controlPointsTuple)
                           {
                               JsonVec3 startPoint = std::get<0>(controlPointsTuple),
                                        controlPoint = std::get<1>(controlPointsTuple),
                                        endPoint = std::get<2>(controlPointsTuple);
                               return std::make_tuple(startPoint, controlPoint, endPoint);
                           });
        }

        return result;
    }

    std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>>
    LevelSerializationHelper::RestoreRouteControlPoints(const std::unordered_map<std::string, std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>> &routesData) const
    {
        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> result;
        result.reserve(routesData.size());
        for (const auto &[routeName, route] : routesData)
        {
            result[routeName] = {};
            std::transform(route.cbegin(),
                           route.cend(),
                           std::inserter(result[routeName], result[routeName].begin()),
                           [](const auto &parsedRouteTuple)
                           {
                               const auto &first = std::get<0>(parsedRouteTuple);
                               const auto &second = std::get<1>(parsedRouteTuple);
                               const auto &third = std::get<2>(parsedRouteTuple);
                               const glm::vec3 &start = glm::vec3(first.x, first.y, first.z);
                               const glm::vec3 &control = glm::vec3(second.x, second.y, second.z);
                               const glm::vec3 &end = glm::vec3(third.x, third.y, third.z);
                               return std::make_tuple(start, control, end);
                           });
        }

        return result;
    }

    std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>>
    LevelSerializationHelper::PrepareTowersData(const std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> &towersData) const
    {
        std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>> result;
        result.reserve(towersData.size());

        std::transform(towersData.cbegin(), towersData.cend(),
                       std::inserter(result, result.begin()),
                       [](const auto &towerPair)
                       {
                           const auto &position = std::get<0>(towerPair.second),
                                      scale = std::get<1>(towerPair.second);
                           return std::make_pair(towerPair.first, std::make_tuple(position, scale));
                       });

        return result;
    }

    std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>>
    LevelSerializationHelper::RestoreTowers(const std::unordered_map<std::string, std::tuple<JsonVec3, JsonVec3>> &towersData) const
    {
        std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> result;
        result.reserve(towersData.size());

        std::transform(towersData.cbegin(), towersData.cend(),
                       std::inserter(result, result.begin()),
                       [](const auto &towerPair)
                       {
                           const auto &first = std::get<0>(towerPair.second),
                                      second = std::get<1>(towerPair.second);

                           const glm::vec3 &position = glm::vec3(first.x, first.y, first.z);
                           const glm::vec3 &scale = glm::vec3(second.x, second.y, second.z);
                           return std::make_pair(towerPair.first, std::make_tuple(position, scale));
                       });

        return result;
    }
}
