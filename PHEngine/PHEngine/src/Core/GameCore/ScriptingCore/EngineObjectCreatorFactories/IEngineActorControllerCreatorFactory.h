#pragma once

#include <glm/vec3.hpp>
#include <stdint.h>

#include <cstdint>
#include <memory>
#include <string>

namespace EngineCore {
class Scene;
namespace Scripts {
class IEngineActorControllerCreatorFactory {
public:
    virtual void CreateActorController(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::string& actorName,
        const std::string& actorControllerTypeName,
        const std::string& jsonParamStr) const
        = 0;
};
} // namespace Scripts
} // namespace EngineCore
