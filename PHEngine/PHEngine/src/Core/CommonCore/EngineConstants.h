#pragma once

#include <cstdint>
#include <string>

namespace EngineConstants {
enum eStencilValues { DEFAULT = 0, BLOOM = 1, OUTLINE = 2 };

extern const int32_t c_maxInstancesPerInstanceBatch;
extern const std::string c_gameThreadName;
extern const std::string c_renderThreadName;
extern const std::string c_luaThreadName;
} // namespace EngineConstants
