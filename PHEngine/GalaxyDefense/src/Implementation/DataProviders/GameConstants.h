#pragma once

#include <glm/vec3.hpp>
#include <stdint.h>

#include <cstdint>

namespace Game::Constants {
extern const float c_spaceshipSize;
extern const int32_t c_spaceshipFontSize;
extern const int32_t c_extraPathPerSideCount;
extern const uint32_t c_spawnSpaceshipTimeoutMs;
extern const float c_portalSize;
extern const uint32_t c_dmgTextShowDuration;
extern const uint32_t c_shakeDurationMs;
extern const glm::vec3 c_barrierPillarScale;
extern const float c_barrierExclusionRadiusFromDestination;
extern const glm::vec3 c_lootSize;
} // namespace Game::Constants
