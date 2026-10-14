#pragma once

#include <glm/ext/quaternion_float.hpp>
#include <glm/vec3.hpp>

#include <map>
#include <string>
#include <vector>

namespace MeshLoader {

struct FrameBase {
    float Time;
};

struct FrameRotation : public FrameBase {
    glm::quat Rotation;
};

struct FrameTranslation : public FrameBase {
    glm::vec3 Translation;
};

struct FrameScale : public FrameBase {
    glm::vec3 Scale;
};

struct AnimationSequenceData {
    std::vector<FrameRotation> RotationFrames;
    std::vector<FrameTranslation> TranslationFrames;
    std::vector<FrameScale> ScaleFrames;
};

struct AnimationMappingData {
    using NodeAnimationBinding_t = std::map<std::string /* Node name */, AnimationSequenceData>;

    NodeAnimationBinding_t NodeAnimationBindings;

    float AnimationDuration;
};
} // namespace MeshLoader
