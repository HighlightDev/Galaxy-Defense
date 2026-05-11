#pragma once

namespace Game {
enum class eMainPlayerStatusType {
    CRYSTALS_COUNT_CHANGED = 0,
    DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED = 1,
    SELECTED_TOWER_CHANGED = 2,
    TOWERS_COUNT_CHANGED = 3
};

enum class eLevelProgressStatusType { NONE, CURRENT_STAGE_CHANGED, REQUIREMENT_TRACKERS_STATUS_CHANGED };
} // namespace Game
