#pragma once

#include "ILevelRequirementTracker.h"

namespace Game {
class MissedSpaceshipsTracker : public ILevelRequirementTracker {
    const int32_t mMissSpaceshipsCountToFailTracker{0};

    int32_t mInitialMissedSpaceshipsCount{0};

    int32_t mMissedSpaceshipsCount{0};

    int32_t mLastMissedSpaceshipsCount{0};

public:
    explicit MissedSpaceshipsTracker(const int32_t missSpaceshipsCountToFailTracker);

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override
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
