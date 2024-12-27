#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class SpaceStationActor;

    class ISpaceStationFactory
    {
    public:
        virtual std::shared_ptr<SpaceStationActor> CreateSpaceStation(const std::shared_ptr<::EngineCore::Scene> &scene,
                                                                      const std::string &towerName,
                                                                      const glm::vec3 &translation,
                                                                      const glm::vec3 &rotation,
                                                                      const glm::vec3 &scale) const = 0;
    };
} // namespace Game
