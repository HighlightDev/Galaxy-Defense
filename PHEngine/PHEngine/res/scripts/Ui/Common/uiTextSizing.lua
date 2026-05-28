-- Shared text-sizing helpers for UI overlays.
--
-- FreeType reports its line height as `face->size->metrics.height` (ascender −
-- descender + line gap). When a UiLabel's container height is smaller than
-- that value, FreeTypeTextMeshCreator falls back to drawing the literal string
-- "container.height < line.height" instead of the requested text (see
-- FreeTypeTextMeshCreator.cpp:69-72).
--
-- The exact ratio between `metrics.height` and the requested point size
-- depends on the font's design (ascender/descender + line gap). The table
-- below stores a calibrated multiplier per known font; `safeLineHeight`
-- looks it up by font name when provided, otherwise uses
-- DEFAULT_HEIGHT_MULTIPLIER, which is wide enough to clear any reasonably
-- proportioned font on the project's roster.
--
-- For pixel-accurate text width, prefer `measureLabelWidthPx` — it goes
-- through the engine's FreeType atlas via `_GetLabelWidthPx` and matches
-- what the font batcher would actually draw.
local UiTextSizing = {}

-- Per-font height multipliers (containerHeight / fontSize). Calibrate by
-- rendering a sample label, comparing `face->size->metrics.height` from a
-- debugger to the requested fontSize, and adding a small safety margin
-- (typically 0.05-0.10) to soak up rounding.
--
-- Fonts not listed fall back to DEFAULT_HEIGHT_MULTIPLIER. Add new entries
-- here rather than overriding callers — that way every overlay sizes the
-- same font consistently.
local FONT_HEIGHT_MULTIPLIER = {
    ["Lora-VariableFont_wght"] = 1.70,
    ["13_5Atom_Sans_Regular"] = 1.55
}

local DEFAULT_HEIGHT_MULTIPLIER = 1.70

-- Returns a container height in pixels that's guaranteed wider than the
-- font's reported line height for the given size. `fontName` is optional —
-- omit it to get the conservative default (works for every font, may waste a
-- few pixels). All existing callers using the 1-arg form keep their previous
-- behaviour.
function UiTextSizing.safeLineHeight(fontSize, fontName)
    assert(fontSize ~= nil and type(fontSize) == "number" and fontSize > 0,
           "UiTextSizing.safeLineHeight: fontSize must be a positive number", debug.traceback())
    local multiplier = DEFAULT_HEIGHT_MULTIPLIER
    if fontName ~= nil then
        assert(type(fontName) == "string", "UiTextSizing.safeLineHeight: fontName must be a string when provided", debug.traceback())
        multiplier = FONT_HEIGHT_MULTIPLIER[fontName] or DEFAULT_HEIGHT_MULTIPLIER
    end
    return math.ceil(fontSize * multiplier)
end

-- Returns the height multiplier table (read-only intent). Useful for tests
-- or tooling that wants to see what's calibrated.
function UiTextSizing.getHeightMultipliers() return FONT_HEIGHT_MULTIPLIER end

-- Pixel-accurate label width via the engine's FreeType atlas. Goes through
-- `_GetLabelWidthPx` which routes to FreeTypeTextMeshCreator::CalcWidth, so
-- the returned number is exactly what the font batcher would lay out —
-- including kerning. First call for a brand-new (font, size) pair triggers
-- atlas allocation in the engine and is heavier; subsequent calls are cheap
-- hashmap lookups.
--
-- Returns 0 for empty text or unknown font/size; callers should plan for
-- this rather than trusting that "0 means tiny".
function UiTextSizing.measureLabelWidthPx(host, fontName, fontSize, text)
    assert(host ~= nil and type(host) == "userdata", "UiTextSizing.measureLabelWidthPx: host must be userdata", debug.traceback())
    assert(fontName ~= nil and type(fontName) == "string" and fontName ~= "",
           "UiTextSizing.measureLabelWidthPx: fontName must be a non-empty string", debug.traceback())
    assert(fontSize ~= nil and type(fontSize) == "number" and fontSize > 0,
           "UiTextSizing.measureLabelWidthPx: fontSize must be a positive number", debug.traceback())
    text = text or ""
    if text == "" then return 0 end
    -- The C++ FreeTypeFontParams uses int32_t for pixelSize, so round here
    -- rather than letting Lua's number→int truncation drop a decimal.
    return _GetLabelWidthPx(host, fontName, math.floor(fontSize + 0.5), text)
end

return UiTextSizing
