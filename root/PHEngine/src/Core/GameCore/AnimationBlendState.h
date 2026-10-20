#pragma once

#include <string>

namespace EngineCore {

struct AnimationBlendState {
    /* src is the main animation */
    std::string SrcName;
    /* dst animation is used when blending of animations is being occurred */
    std::string DstName;

    /* src is the main animation time counter */
    float SrcTime = 0.0f;
    /* dst time is used when blending of animations is being calculated */
    float DstTime = 0.0f;

    bool bTransitionEnabled = false;
    float TransitionValue = 0.0f;

    bool operator==(const AnimationBlendState&) const = default;
};

} // namespace EngineCore
