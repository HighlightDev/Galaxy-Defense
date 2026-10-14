#pragma once

#include "IEngineActorCreatorFactory.h"

namespace EngineCore {
class Scene;
namespace Scripts {
class ActorCreatorFactory : public IEngineActorCreatorFactory {
public:
    int32_t CreateActor(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::string& actorName,
        const glm::vec3& rootTranslation,
        const glm::vec3& rootEulerRotationAngles,
        const glm::vec3& rootScale,
        const std::string& jsonParamStr) const override;
};
} // namespace Scripts
} // namespace EngineCore
