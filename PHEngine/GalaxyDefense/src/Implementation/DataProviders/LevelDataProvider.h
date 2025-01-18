#pragma once

#include <cstdint>
#include <stdint.h>

namespace Game
{
    class LevelDataProvider
    {
        int32_t mCurrentStageSurvivedEnemySpaceshipsCount{0};
        
    private:
        LevelDataProvider() = default;

    public:
        static LevelDataProvider *GetInstance();

        int32_t GetCurrentStageSurvivedEnemySpaceshipsCount() const;

        void SetCurrentStageSurvivedEnemySpaceshipsCount(const int32_t count);
    };
}
