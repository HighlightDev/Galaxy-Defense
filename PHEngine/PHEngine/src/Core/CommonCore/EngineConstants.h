#pragma once

#include <string.h>

#include <cstdint>

namespace EngineConstants {
enum eStencilValues { DEFAULT = 0, BLOOM = 1, OUTLINE = 2 };

extern const int32_t c_maxInstancesPerInstanceBatch;
} // namespace EngineConstants
