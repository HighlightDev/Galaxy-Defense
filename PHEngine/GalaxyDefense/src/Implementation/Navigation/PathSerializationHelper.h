#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <tuple>
#include <unordered_map>

namespace Game
{
    struct PathSerializationHelper
    {
        std::string DumpRouteControlPointsToJsonString(const std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> &routeControlPoints);

        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> RestoreRouteControlPointsFromJsonString(const std::string &jsonStr);
    };
}
