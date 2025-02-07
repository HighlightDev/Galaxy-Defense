#pragma once

#include "ILevelRequirementTracker.h"

#include <memory>

namespace Game {
class ILevelProgressRequirement {
public:
    virtual std::unique_ptr<ILevelRequirementTracker> CreateRequirementTracker() const = 0;

    virtual std::string ToString() const = 0;
};
} // namespace Game
