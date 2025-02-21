#pragma once

#include "IEngineActorControllerCreatorFactory.h"

namespace EngineCore {
class Scene;
namespace Scripts {
class ActorControllerCreatorFactory : public IEngineActorControllerCreatorFactory {
public:
    void CreateActorController(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::string& actorName,
        const std::string& actorControllerTypeName,
        const std::string& jsonParamStr) const override;
};
} // namespace Scripts
} // namespace EngineCore
