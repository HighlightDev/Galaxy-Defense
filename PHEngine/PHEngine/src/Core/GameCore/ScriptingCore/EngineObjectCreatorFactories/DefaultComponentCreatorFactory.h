#pragma once

#include "IEngineComponentCreatorFactory.h"

#include <json/json.hpp>

#include <memory>

namespace EnginePhysics {
struct CollisionShapeBase;
}

namespace EngineCore {
class Scene;
class Component;
struct ComponentData;

namespace Scripts {
class DefaultComponentCreatorFactory : public IEngineComponentCreatorFactory {
public:
    int32_t CreateComponent(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const int32_t actorObjectId,
        const std::string& componentType,
        const std::string& componentDataJsonStr) const override;

    int32_t CreatePlanarReflectionComponent(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp, const std::string& componentDataJsonStr) const override;

private:
    std::shared_ptr<ComponentData> CreateComponentData(
        const std::shared_ptr<::EngineCore::Scene>& sceneSp,
        const std::string& componentType,
        const std::string& componentDataJsonStr) const;

    std::shared_ptr<::EnginePhysics::CollisionShapeBase>
    CreateCollisionShapeFromJson(const nlohmann::json& shapeRoot, const std::string& collisionShapeName) const;

    void PostProcessCreatedComponent(
        const std::shared_ptr<::EngineCore::Scene>& sceneSp,
        const std::shared_ptr<::EngineCore::Component>& component,
        const std::string& componentType) const;
};
} // namespace Scripts
} // namespace EngineCore
