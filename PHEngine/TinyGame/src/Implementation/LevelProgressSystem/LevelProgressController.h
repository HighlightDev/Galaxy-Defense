#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>

namespace Game
{
    class LevelProgressDescriptor
    {
    };

    class LevelProgressController : public ITickable
    {
        std::unique_ptr<LevelProgressDescriptor> mLvlProgressDescriptor;

    public:
        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddLevelProgressDescriptor(const std::unique_ptr<LevelProgressDescriptor> &lvlProgressDescriptor);
    };
}
