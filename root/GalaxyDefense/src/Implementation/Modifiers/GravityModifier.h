#pragma once

#include "IModifiable.h"

#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
class Actor;
class RuntimeGeneratedQuadraticBezierCurveComponent;
} // namespace EngineCore

namespace Graphics {
class IMaterial;
}

namespace Game {
class SpaceshipActor;
class MissileActor;

class GravityModifier : public IModifiable {
    std::weak_ptr<SpaceshipActor> mOwnerWp;

    std::weak_ptr<MissileActor> mMissileWp;

    glm::vec3 mGravityCenterPosition;

    float mGravityPower;

    // Original ship scale captured the first time spaghettification is applied, restored when the modifier is removed
    // (ships are pooled and reused, so they must never be left deformed).
    glm::vec3 mOriginalScale{1.0f};

    bool mIsScaleCaptured{false};

    // Set once the ship has been consumed by the collapsing singularity, to avoid re-triggering its death every tick.
    bool mHasConsumedOwner{false};

    // Bezier tether curve from the singularity to the captured ship: an identity-transform host actor owns it, drawn
    // with a shared material. Created lazily on the first tick, removed in OnPreRemoved.
    std::weak_ptr<EngineCore::Actor> mTetherHostWp;

    std::shared_ptr<Graphics::IMaterial> mTetherMaterial;

    std::shared_ptr<EngineCore::RuntimeGeneratedQuadraticBezierCurveComponent> mTetherComponent;

public:
    GravityModifier(
        const std::weak_ptr<SpaceshipActor>& owner,
        const std::weak_ptr<MissileActor>& missile,
        const glm::vec3& gravityCenterPosition);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    bool IsExpired() const override;

    void OnPreRemoved() override;

    void SetGravityPower(const float power);

    void SetTether(const std::shared_ptr<EngineCore::Actor>& hostActor, const std::shared_ptr<Graphics::IMaterial>& material);

private:
    // Stretches the ship toward the singularity (spaghettification) the closer it gets to the gravity center.
    void ApplySpaghettification(const std::shared_ptr<SpaceshipActor>& spaceship, const float distanceToCenter);

    void RestoreOriginalScale();

    // Lazily creates (if needed) and updates the bezier tether so it bows from the ship to the singularity core.
    void UpdateTether(const std::shared_ptr<SpaceshipActor>& spaceship, const glm::vec3& shipPosition);

    void DestroyTether();
};
} // namespace Game