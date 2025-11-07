#pragma once

#include "ILevelRequirementTracker.h"

namespace Game {
class MissedSpaceshipsTracker : public ILevelRequirementTracker {
    const int32_t mMissSpaceshipsCountToFailTracker{0};

    int32_t mInitialMissedSpaceshipsCount{0};

    int32_t mMissedSpaceshipsCount{0};

    int32_t mLastMissedSpaceshipsCount{0};

    std::string mRequirementHint;

public:
    explicit MissedSpaceshipsTracker(const int32_t missSpaceshipsCountToFailTracker, const std::string& requirementHint);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override
    {
    }

    void Init() override;

    bool IsRequirementAchived() const override;

    bool IsRequirementFailed() const override;

    std::string GetName() const override;

    std::unordered_map<std::string, std::string> SerializeParameters() const override;

    bool CheckIfRequirementIsDirty(const bool unsetDirtyFlag) override;

    eLevelRequirementTrackerType GetType() const override;

private:
    void SetMissedSpaceshipsCount(const int32_t value);
};
} // namespace Game
