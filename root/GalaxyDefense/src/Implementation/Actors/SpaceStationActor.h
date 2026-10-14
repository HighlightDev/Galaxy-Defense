#pragma once

#include "Core/GameCore/Actor.h"
#include "IHighlightable.h"
#include "Implementation/ActorLeveling/SpaceStationLevel.h"

using namespace EngineCore;

namespace EngineCore {
class StaticMeshComponent;
}

namespace Game {
enum class eSpaceStationActivityState { IDLE, ACTIVE };

class SpaceStationActor : public Actor, public IHighlightable {

    float mShootCooldown{0.0f};

    bool mIsRayActive{false};

    eSpaceStationActivityState mSpacestationState{eSpaceStationActivityState::IDLE};

    std::shared_ptr<::EngineCore::StaticMeshComponent> mMainMeshComponent;

    std::shared_ptr<::EngineCore::StaticMeshComponent> mRadiusMarkerComponent;

    std::shared_ptr<SpaceStationLevel> mSpaceStationLevel;

    std::shared_ptr<EngineObjectProperty<float>> mShootRadiusProperty;

    glm::vec3 mSpaceStationSize;

public:
    SpaceStationActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void ChangeHighlightState(const bool isHighlightEnabled) override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();

    void SetState(const eSpaceStationActivityState spacestationState);

    eSpaceStationActivityState GetState() const;

    void SetSpaceStationLevel(const std::shared_ptr<SpaceStationLevel>& spaceStationLevel);

    const std::shared_ptr<SpaceStationLevel>& GetSpaceStationLevel() const;

    void SetIsRayActive(const bool value);

    bool GetIsRayActive() const;

    void SetMainMeshComponent(const std::shared_ptr<::EngineCore::StaticMeshComponent>& mainMeshComponent);

    void SetRadiusMarkerComponent(const std::shared_ptr<::EngineCore::StaticMeshComponent>& radiusMarkerComponent);

    void SetSpaceStationSize(const glm::vec3& size);

    const glm::vec3& GetSpaceStationSize() const;

    void SetIsEnabled(bool isEnabled) override;

    void SetIsOutlineApplied(const bool isOutlineApplied);

    bool GetIsOutlineApplied() const;

    void SetIsRadiusMarkerActive(const bool isRadiusMarkerActive);

    bool GetIsRadiusMarkerActive() const;
};
} // namespace Game
