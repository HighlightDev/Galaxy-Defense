#pragma once

#include "Core/GameCore/Actor.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class SceneComponent;
class StaticMeshComponent;
class RuntimeGeneratedLineComponent;
} // namespace EngineCore

using namespace EngineCore;

namespace Game {
class BarrierActor : public Actor {

    std::vector<std::shared_ptr<::EngineCore::StaticMeshComponent>> mBarrierPillars;

    std::vector<std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>> mBarrierRays;

public:
    BarrierActor(const std::string& gameObjectName, const std::shared_ptr<::EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTime) override;

    void AddBarrierPillarMesh(const std::shared_ptr<::EngineCore::StaticMeshComponent>& meshComponent);

    void AddRayLineMesh(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>& rayComponent);

    bool TrySetBarrierPillarMeshRelativeTransform(
        const int32_t pillarIndex, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    glm::vec3 GetBarrierPillarPosition(const int32_t pillarIndex) const;

    int32_t GetBarrierPillarsCount() const;

    int32_t GetBarrierRaysCount() const;

    std::vector<std::shared_ptr<::EngineCore::StaticMeshComponent>> GetBarrierPillarsMeshComponents() const;
};
} // namespace Game
