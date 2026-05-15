#pragma once

#include <cstdint>

namespace EngineConstants {
enum eStencilValues { DEFAULT = 0, BLOOM = 1, OUTLINE = 2, GUI_SCISSORING = 3 };

extern const int32_t c_maxInstancesPerInstanceBatch;
extern const char* c_gameThreadName;
extern const char* c_renderThreadName;
extern const char* c_luaThreadName;
} // namespace EngineConstants
