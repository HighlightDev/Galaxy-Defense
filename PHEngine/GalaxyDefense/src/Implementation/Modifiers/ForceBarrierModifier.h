#pragma once

#include "IModifiable.h"

#include <memory>

namespace Game {
class BarrierActor;

// Force Barrier shield buff: while the force-barrier ray stays connected to a barrier, this modifier keeps the barrier
// force-shielded (it ignores pillar damage). Mirrors FreezingRayModifier: applied on collision start, marked expired on
// collision end, and reverts its effect in OnPreRemoved.
class ForceBarrierModifier : public IModifiable {
    std::weak_ptr<BarrierActor> mOwnerWp;

    bool mIsExpired{false};

public:
    ForceBarrierModifier(const std::weak_ptr<BarrierActor>& owner);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    void SetIsExpired(const bool isExpired);

    bool IsExpired() const override;

    void OnPreRemoved() override;
};
} // namespace Game
