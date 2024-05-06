#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
    class ACamera;
}

namespace Game
{
    class CombatActorsPoolHandler;

    class SmartPicker
    {
        std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    public:
        explicit SmartPicker(const std::shared_ptr<CombatActorsPoolHandler> &combatActorsPoolHandler);

        glm::vec3 CreateWorldSpaceRayFromScreenSpacePosition(const std::shared_ptr<::EngineCore::ACamera> &camera,
                                                             const glm::ivec2 &screenSpacePosition) const;

        int32_t CastScreenSpaceRayIntoScene(
            const std::shared_ptr<::EngineCore::Scene> &sceneSp,
            const std::shared_ptr<::EngineCore::ACamera> &camera,
            const glm::ivec2 &screenSpacePosition);
    };
}
