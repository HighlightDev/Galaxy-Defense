#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Implementation/ActorLeveling/BarrierLevel.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class SceneComponent;
class StaticMeshComponent;
class ElectricBeamComponent;
} // namespace EngineCore

namespace EnginePhysics {
class PhysicsComponent;
}

namespace Graphics {
class IMaterial;
} // namespace Graphics

using namespace EngineCore;

namespace Game {

class BarrierUiComponent;

enum class eBarrierActivityState { IDLE, ACTIVE };

class BarrierActor : public Actor {

public:
    struct BarrierUiProtoData {
        std::string font;
        uint32_t fontSize;
        std::string text;
        glm::vec3 color;
        glm::ivec2 lineMaxWidthHeight;
        eTextHorizontalAlignmentType textHorizontalAlignment;
        eTextVerticalAlignmentType textVerticalAlignment;
    };

private:
    std::vector<std::shared_ptr<::EngineCore::StaticMeshComponent>> mBarrierPillars;

    std::vector<std::shared_ptr<::EngineCore::ElectricBeamComponent>> mBarrierRays;

    eBarrierActivityState mBarrierState{eBarrierActivityState::IDLE};

    std::shared_ptr<::Graphics::IMaterial> mPillarMaterial;

    std::shared_ptr<::Graphics::IMaterial> mRayMaterial;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> mPillarPhysicsComponents;

    std::vector<BarrierLevel> mPillarLevels;

    uint32_t mNominalPillarHealth{0};

    std::vector<bool> mPillarAlive;

    BarrierUiProtoData mUiProtoData;

    std::vector<std::shared_ptr<BarrierUiComponent>> mUiComponents;

    std::unordered_map<int32_t, std::shared_ptr<GameThreadTimer>> mDamageMessageTimers;

public:
    BarrierActor(const std::string& gameObjectName, const std::shared_ptr<::EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec) override;

    void setBarrierProtoData(const BarrierUiProtoData& protoData);

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

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetPillarPhysicsComponents() const;

    void SetNominalPillarHealth(const uint32_t pillarHealth);

    void TriggerPillarDamage(const int32_t pillarIndex, const uint32_t damage);

    int32_t FindPillarIndexByPhysDescriptorId(const int32_t physDescriptorId) const;

    std::vector<std::pair<glm::vec3, glm::vec3>> GetBarrierActiveRaysWorldPositions() const;

private:
    void DestroyPillar(const int32_t pillarIndex);

    void UpdateRaysConnectivity();

    void UpdateHealthBars();

    bool AreAllPillarsDestroyed() const;
};
} // namespace Game
