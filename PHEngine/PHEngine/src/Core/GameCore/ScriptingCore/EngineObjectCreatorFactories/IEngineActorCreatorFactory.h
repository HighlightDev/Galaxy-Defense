#pragma once

#include <glm/vec3.hpp>
#include <stdint.h>

#include <cstdint>
#include <memory>
#include <string>

namespace EngineCore {
class Scene;
namespace Scripts {
class IEngineActorCreatorFactory {
public:
    virtual int32_t CreateActor(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::string& actorName,
        const glm::vec3& rootTranslation,
        const glm::vec3& rootEulerRotationAngles,
        const glm::vec3& rootScale,
        const std::string& jsonParamStr) const
        = 0;
};
} // namespace Scripts
} // namespace EngineCore
