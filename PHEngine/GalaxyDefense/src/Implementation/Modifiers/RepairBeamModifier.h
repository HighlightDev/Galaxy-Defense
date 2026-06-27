#pragma once

#include "IModifiable.h"

#include <memory>

namespace Game {
class BarrierActor;

// Repair-beam heal buff: while the repair beam stays connected to a barrier, this modifier heals the barrier's
// most-damaged pillar over time. Mirrors ForceBarrierModifier: applied on collision start, marked expired on collision
// end. The healing it has already applied is permanent, so OnPreRemoved has nothing to revert.
class RepairBeamModifier : public IModifiable {
    std::weak_ptr<BarrierActor> mOwnerWp;

    // Fractional HP carried between ticks until at least 1 whole HP can be applied.
    float mHealAccumulator{0.0f};

    bool mIsExpired{false};

public:
    RepairBeamModifier(const std::weak_ptr<BarrierActor>& owner);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    void SetIsExpired(const bool isExpired);

    bool IsExpired() const override;

    void OnPreRemoved() override;
};
} // namespace Game
