#pragma once

namespace EngineCore {
enum class eTextHorizontalAlignmentType { LEFT, CENTER, RIGHT };
enum class eTextVerticalAlignmentType { TOP, CENTER, BOTTOM };

enum eFontFlags {
    LeftAligned = 1 << 1,
    RightAligned = 1 << 2,
    CenterAligned = 1 << 3,
    WordWrap = 1 << 4,
    Underlined = 1 << 5,
    Bold = 1 << 6,
    Italic = 1 << 7,
    Indented = 1 << 8,
    HorizontalLayout = 1 << 9
};

enum class eTextGradientColorType { NONE, VERTICAL, HORIZONTAL };
} // namespace EngineCore