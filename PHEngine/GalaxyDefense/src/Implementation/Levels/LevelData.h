#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <glm/vec3.hpp>

namespace Game
{
    struct LevelData
    {
        std::string LevelName;

        std::unordered_map<std::string, std::tuple<glm::vec3/*position*/, glm::vec3/*scale*/>> TowersData;

        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3/*start*/, glm::vec3/*control point*/, glm::vec3/*end*/>>> RoutesData;

        bool isDataValid() const;
    };
}
