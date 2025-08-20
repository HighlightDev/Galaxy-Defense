#pragma once

#include <freetype/freetype.h>

namespace EngineCore::GUI {
enum eFTLoadFlags {
    LoadDefault = FT_LOAD_DEFAULT,
    LoadNoScale = FT_LOAD_NO_SCALE,
    LoadNoHinting = FT_LOAD_NO_HINTING,
    LoadRender = FT_LOAD_RENDER,
    LoadNoBitmap = FT_LOAD_NO_BITMAP,
    LoadVerticalLayout = FT_LOAD_VERTICAL_LAYOUT,
    LoadForceAutohint = FT_LOAD_FORCE_AUTOHINT,
    LoadCropBitmap = FT_LOAD_CROP_BITMAP,
    LoadPedantic = FT_LOAD_PEDANTIC,
    LoadIgnoreGlobalAdvanceWidth = FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH,
    LoadNoRecurse = FT_LOAD_NO_RECURSE,
    LoadIgnoreTransform = FT_LOAD_IGNORE_TRANSFORM,
    LoadMonochrome = FT_LOAD_MONOCHROME,
    LoadLinearDesign = FT_LOAD_LINEAR_DESIGN,
    LoadSbitsOnly = FT_LOAD_SBITS_ONLY,
    LoadNoAutohint = FT_LOAD_NO_AUTOHINT,
    LoadColor = FT_LOAD_COLOR,
    LoadComputeMetrics = FT_LOAD_COMPUTE_METRICS,
    LoadBitmapMetricsOnly = FT_LOAD_BITMAP_METRICS_ONLY,
    LoadNoSvg = FT_LOAD_NO_SVG
};
}