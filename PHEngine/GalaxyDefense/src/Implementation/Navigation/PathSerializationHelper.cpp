#include "PathSerializationHelper.h"
#include "Core/CommonCore/JsonHelper.h"

#include <algorithm>
#include <json/json.hpp>

namespace Game
{
    std::string PathSerializationHelper::DumpRouteControlPointsToJsonString(const std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> &routeControlPoints)
    {
        std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>> result;
        result.reserve(routeControlPoints.size());

        std::transform(routeControlPoints.cbegin(), routeControlPoints.cend(),
                       std::back_inserter(result),
                       [](const auto &controlPointsTuple)
                       {
                           JsonVec3 startPoint = std::get<0>(controlPointsTuple),
                                    controlPoint = std::get<1>(controlPointsTuple),
                                    endPoint = std::get<2>(controlPointsTuple);
                           return std::make_tuple(startPoint, controlPoint, endPoint);
                       });

        nlohmann::json jsonObj;
        jsonObj["routes"] = result;

        return jsonObj.dump();
    }

    std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> PathSerializationHelper::RestoreRouteControlPointsFromJsonString(const std::string &jsonStr)
    {
        std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>> parsedRoutePoints;
        const auto &jsonObj = nlohmann::json::parse(jsonStr);
        if (jsonObj.contains("routes"))
        {
            parsedRoutePoints = jsonObj["routes"].get<std::vector<std::tuple<JsonVec3, JsonVec3, JsonVec3>>>();
        }

        std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> result;
        result.reserve(parsedRoutePoints.size());
        std::transform(parsedRoutePoints.cbegin(),
                       parsedRoutePoints.cend(),
                       std::back_inserter(result),
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

        return result;
    }
}
