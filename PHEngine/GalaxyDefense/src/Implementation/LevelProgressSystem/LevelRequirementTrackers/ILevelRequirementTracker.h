#pragma once

#include "Core/GameCore/ITickable.h"

#include <string>
#include <unordered_map>

namespace Game
{
    class ILevelRequirementTracker
        : public ITickable
    {
    public:
        virtual void Init() = 0;

        virtual bool IsRequirementAchived() const = 0;

        virtual std::string GetName() const = 0;

        virtual std::unordered_map<std::string, std::string> SerializeParameters() const = 0;

        virtual bool CheckIfRequirementIsDirty(const bool unsetDirtyFlag) = 0;
    };
}
