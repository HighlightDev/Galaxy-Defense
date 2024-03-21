#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <tuple>

namespace Game
{
    struct PathSerializationHelper
    {
        std::string DumpRouteControlPointsToJsonString(const std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> &routeControlPoints);

        std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> RestoreRouteControlPointsFromJsonString(const std::string& jsonStr);
    };
}
