#pragma once

namespace Game {
enum class eMissileType { NONE, BOMB, FREEZING_BOMB, ELECTRO_RAY, BLACK_HOLE, FREEZING_RAY, PLASMA_BOMB, REPAIR_BEAM };

enum class eRayType { ELECTRO_RAY, FREEZING_RAY, REPAIR_BEAM };

enum class eCollisionActionType { COLLISION_STARTED, COLLISION_FINISHED };
} // namespace Game
