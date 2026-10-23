#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/GUI/Common/TextEnums.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>

#include <cstdint>
#include <vector>

namespace EngineCore {
struct PendingPrimitiveTransform {
    glm::mat4 worldMatrix;
    glm::mat4 outlineMatrix;
    BoundingBox3D transformedBoundingBox;
    glm::vec3 originPosition;
};

struct PendingUiItemBaseUpdates {
    int32_t canvasUid;
    bool isVisible;
    std::vector<int32_t> zPath;
    glm::vec2 normTranslation;
    glm::vec2 normScale;
    size_t width;
    size_t height;
#ifdef DEBUG
    bool isHiddenForDebugging;
#endif
    bool isGuiScissorsSlave;
    bool isGuiScissorsMaster;
    bool canBloomBeApplied;
};

struct PendingUiRectangleUpdates {
    int32_t canvasUid;
    glm::vec3 color;
    float opacity;
    float borderRadius;
    float blurMix;
    bool isRoundTop;
    bool isRoundBottom;
    bool applyBlur;
};

struct PendingUiLabelUpdates {
    int32_t CanvasUid;
    std::string Text;
    float Opacity;
    int32_t FontSize;
    glm::ivec2 TextLineWidthHeight;
    glm::vec3 TextColor;
    eTextHorizontalAlignmentType TextHorizontalAlignment;
    eTextVerticalAlignmentType TextVerticalAlignment;
    eTextGradientColorType TextGradientColorType;
    glm::vec3 GradientTextColorStart;
    glm::vec3 GradientTextColorEnd;
};
} // namespace EngineCore
