#pragma once

#include "Core/GameCore/ITickable.h"

#include <string>

namespace Game {
class ILevelRequirementTracker : public ITickable {
public:
    virtual bool IsRequirementAchived() const = 0;

    virtual std::string GetName() const = 0;
};
} // namespace Game
