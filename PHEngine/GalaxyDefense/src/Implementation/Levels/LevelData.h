#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Game {
struct LevelData {
    std::string LevelName;

    glm::vec2 LevelBoundaryMin;

    glm::vec2 LevelBoundaryMax;

    std::unordered_map<std::string, std::tuple<glm::vec3 /*position*/, glm::vec3 /*scale*/>> TowersData;

    std::unordered_map<std::string, std::vector<std::tuple<glm::vec3 /*start*/, glm::vec3 /*control point*/, glm::vec3 /*end*/>>>
        RoutesData;

    std::unordered_map<std::string, std::vector<glm::vec3>> BarriersData;

    bool isDataValid() const;

private:
    bool IsLevelBoundariesValid() const;
};
} // namespace Game
