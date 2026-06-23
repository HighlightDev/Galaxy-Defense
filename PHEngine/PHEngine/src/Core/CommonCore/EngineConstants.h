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
// constexpr so it can size the fixed std::array job swap-chains (a template/array size needs a compile-time constant).
constexpr uint32_t c_renderThreadJobsPoolsSize = 2000;
extern const char* c_gameThreadName;
extern const char* c_renderThreadName;
extern const char* c_luaThreadName;
} // namespace EngineConstants
