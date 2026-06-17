#pragma once

namespace Graphics {
enum class eRenderPassType {
    SHADOW_DEPTH_PASS,
    PLANAR_REFLECTION_PASS,
    OUTLINE_PASS,
    DEPTH_PRE_PASS,
    DEFERRED_BASE_PASS,
    DEFERRED_LIGHT_PASS,
    FORWARD_BASE_PASS,
    GUI_PASS
};
}