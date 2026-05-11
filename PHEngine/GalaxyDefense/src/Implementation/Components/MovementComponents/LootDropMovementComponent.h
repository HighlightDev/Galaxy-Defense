#pragma once

#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"

#include <glm/vec3.hpp>

using namespace EngineCore;

namespace Game {

enum class eLootDropPhase { FALLING, BOUNCING, DONE };

// Moves the loot actor straight down by a configurable fall distance,
// then performs a small upward sine-shaped bounce before stopping.
class LootDropMovementComponent : public NoPhysicsMovementComponent {
    float mFallDistance{5.0f};
    float mFallenSoFar{0.0f};

    float mBounceAmplitude{1.0f};
    float mBounceDuration{0.6f};
    float mBounceTimeElapsed{0.0f};

    glm::vec3 mBounceBasePosition{0.0f};

    eLootDropPhase mPhase{eLootDropPhase::FALLING};

public:
    explicit LootDropMovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData);

    void Move(const float deltaTimeSec) override;

    void SetFallDistance(const float dist);
    float GetFallDistance() const;

    void SetBounceAmplitude(const float amplitude);
    float GetBounceAmplitude() const;

    void SetBounceDuration(const float duration);
    float GetBounceDuration() const;

    bool IsDone() const;

    void Reset();
};

} // namespace Game
