#pragma once

#include <cstdint>

namespace EngineConstants {
enum eStencilValues : uint32_t {
    SCENE_DEFAULT = 0x1,
    GUI_SCISSORING = SCENE_DEFAULT << 1,
    BLOOM = GUI_SCISSORING << 1,
    OUTLINE = BLOOM << 1,
};

extern const int32_t c_maxInstancesPerInstanceBatch;
extern const char* c_gameThreadName;
extern const char* c_renderThreadName;
extern const char* c_luaThreadName;
} // namespace EngineConstants
