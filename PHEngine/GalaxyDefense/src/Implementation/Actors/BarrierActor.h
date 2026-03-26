#pragma once

#include "Core/GameCore/Actor.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class SceneComponent;
class StaticMeshComponent;
class ElectricBeamComponent;
} // namespace EngineCore

namespace EnginePhysics {
struct CollisionCompoundShape;
}

namespace Graphics {
class IMaterial;
} // namespace Graphics

using namespace EngineCore;

namespace Game {

enum class eBarrierActivityState { IDLE, ACTIVE };

class BarrierActor : public Actor {

    std::vector<std::shared_ptr<::EngineCore::StaticMeshComponent>> mBarrierPillars;

    std::vector<std::shared_ptr<::EngineCore::ElectricBeamComponent>> mBarrierRays;

    eBarrierActivityState mBarrierState{eBarrierActivityState::IDLE};

    std::shared_ptr<::Graphics::IMaterial> mPillarMaterial;

    std::shared_ptr<::Graphics::IMaterial> mRayMaterial;

    std::shared_ptr<::EnginePhysics::CollisionCompoundShape> mCompoundShape;

public:
    BarrierActor(const std::string& gameObjectName, const std::shared_ptr<::EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec) override;

    void SetBarrierMaterials(
        const std::shared_ptr<::Graphics::IMaterial>& pillarMaterial, const std::shared_ptr<::Graphics::IMaterial>& rayMaterial);

    void CreateNewBarrierPillar(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

    void RemoveAllBarrierPillars();

    void AddBarrierPillarMesh(const std::shared_ptr<::EngineCore::StaticMeshComponent>& meshComponent);

    void AddRayLineMesh(const std::shared_ptr<::EngineCore::ElectricBeamComponent>& rayComponent);

    bool TrySetBarrierPillarMeshRelativeTransform(
        const int32_t pillarIndex, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    glm::vec3 GetBarrierPillarPosition(const int32_t pillarIndex) const;

    int32_t GetBarrierPillarsCount() const;

    int32_t GetBarrierRaysCount() const;

    std::vector<std::shared_ptr<::EngineCore::StaticMeshComponent>> GetBarrierPillarsMeshComponents() const;

    void SetState(const eBarrierActivityState barrierState);

    eBarrierActivityState GetState() const;

    void SetCompoundShape(const std::shared_ptr<::EnginePhysics::CollisionCompoundShape>& compoundShape);
};
} // namespace Game
