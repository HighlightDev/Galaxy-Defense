#pragma once

#include <stdint.h>

#include <cstdint>
#include <memory>
#include <string>

namespace EngineCore {
class Scene;
namespace Scripts {
class IEngineComponentCreatorFactory {
public:
    virtual void CreateComponent(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const int32_t actorObjectId,
        const std::string& componentType,
        const std::string& componentDataJsonStr) const = 0;

    virtual void CreatePlanarReflectionComponent(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp, const std::string& componentDataJsonStr) const = 0;
};
} // namespace Scripts
} // namespace EngineCore
