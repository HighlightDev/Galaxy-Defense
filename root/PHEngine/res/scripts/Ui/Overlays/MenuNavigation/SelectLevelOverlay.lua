--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]] --
local function setup()
    local slash = package.config:sub(1, 1)
    assert(slash ~= nil and type(slash) == "string" and slash ~= "")
    local pattern = ""
    if slash == "/" then
        pattern = "(.*/)"
    elseif slash == "\\" then
        pattern = "(.*\\)"
    end
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match(pattern)
    if pathToCurrentScript ~= nil then
        local unixLikePath = pathToCurrentScript:gsub("\\", "/")
        unixLikePath = unixLikePath:gsub("//", "/")
        local _, endindex = string.find(unixLikePath, "scripts/")
        unixLikePath = string.sub(unixLikePath, 1, endindex)
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]

local EventsHelper = require("Ui/Core/eventsHelper")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiScrollList = require("Ui/Core/uiScrollList")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")
local UiLabel = require("Ui/Core/uiLabel")
local json = require("Ui/Core/3rdparty/json")

-- Level select screen modelled after UiMockup/menu-levels-centered.jsx. Same Galaxy Defense visual
-- language as MainMenuOverlay / SettingsOverlay: corner brackets, HUD chips, title block and a
-- scrollable list of bracketed rows (one per level file) with a cyan accent on hover.
SelectLevelOverlay = {}

local Combat = Styles.Combat
local M = Styles.MainMenu

local FONT = "JetBrainsMono-VariableFont_wght"

-- ZOrder layers (local to the parent widget).
local Z_SCRIM = 1
local Z_CORNER = 5
local Z_HUD = 8
local Z_TITLE = 10
local Z_LIST = 12
local Z_ROW_BG = 13
local Z_ROW_FG = 14

-- List geometry, from the mockup's .lvl-list-wrap / .lvl-row (1600x900 reference)
local LIST_WIDTH = 600
local LIST_SCROLLBAR_GAP = 10 -- .lvl-list padding-right, keeps the rows clear of the scrollbar
local ROW_WIDTH = LIST_WIDTH - LIST_SCROLLBAR_GAP
local ROW_PAD_V = 16
local ROW_PAD_H = 22
local ROW_GAP = 14 -- gap between the row columns
local ROW_SPACING = 8 -- gap between the rows
local ROW_INDEX_WIDTH = 32
local ROW_BRACKET_WIDTH = 14
local ROW_GO_WIDTH = 84
local ROW_NAME_FONT_SIZE = 16
local ROW_NAME_WIDTH = ROW_WIDTH - 2 * ROW_PAD_H - ROW_INDEX_WIDTH - 2 * ROW_BRACKET_WIDTH - ROW_GO_WIDTH - 4 * ROW_GAP

local safeLineHeight = UiTextSizing.safeLineHeight

local function makeLabel(host, debugName, text, fontSize, colorHex, hAlign, vAlign)
    local label = UiLabel:new(host, FONT, debugName)
    label.__initText = text
    label.__initFontSize = fontSize
    label.__initColor = colorHex
    label.__initHAlign = hAlign or UiLabel.TextHorizontalAlignmentType.LEFT
    label.__initVAlign = vAlign or UiLabel.TextVerticalAlignmentType.CENTER
    label.safeLineHeight = safeLineHeight(fontSize)
    return label
end

local function applyLabelInit(label)
    label:setText(label.__initText)
    label:setFontSize(label.__initFontSize)
    label:setTextColorHexValue(label.__initColor)
    label:setTextHorizontalAlignment(label.__initHAlign)
    label:setTextVerticalAlignment(label.__initVAlign)
end

-- Engine-side SetIsVisible propagates a parent's visibility to all its children, overriding their own state, and
-- the Lua-side cached value may lag behind. Mark the property dirty explicitly so the desired state is always re-sent.
local function forceWidgetVisibility(widget, isVisible)
    widget.properties.visible.value = isVisible
    widget.properties.visible.dirty = true
end

local function ParseLevelsList(levelsListJson)
    return json.decode(levelsListJson)["level_files"]
end

local function getLevelDisplayName(levelFileName)
    local nameWithoutExtension = levelFileName:gsub("%.[^%.]+$", "")
    return nameWithoutExtension
end

-- A label renders a "container.width < line.width" placeholder instead of a text wider than the label, so a long
-- name is cut with an ellipsis (the mockup's text-overflow: ellipsis)
local function fitTextToWidth(host, text, fontSize, maxWidth)
    local function measure(t)
        return UiTextSizing.measureLabelWidthPx(host, FONT, fontSize, t)
    end
    if measure(text) <= maxWidth then
        return text
    end
    local chars = {}
    for _, codePoint in utf8.codes(text) do
        chars[#chars + 1] = utf8.char(codePoint)
    end
    for count = #chars - 1, 1, -1 do
        local candidate = table.concat(chars, "", 1, count) .. "..."
        if measure(candidate) <= maxWidth then
            return candidate
        end
    end
    return "..."
end

function SelectLevelOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    -- ----------------------------------------------------------------------
    -- Canvas + overlay
    -- ----------------------------------------------------------------------
    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "SelectLevelCanvas")
    canvas:subscribeOnLuaProxyReady(function(h)
        canvas:setCanvasZOrder(100)
        _InitializeCanvasInputSystem(h, canvas.luaProxyId)
        canvas:addFadeInAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        canvas:addFadeOutAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local overlay = UiOverlay:createOverlay(host, "SelectLevelOverlay", canvas)

    -- ----------------------------------------------------------------------
    -- Scrim — darken the space backdrop so the text reads cleanly.
    -- ----------------------------------------------------------------------
    local scrim = UiRectangle:new(host, "SLScrim")
    overlay:addWidget(scrim)

    -- ----------------------------------------------------------------------
    -- Corner brackets — 8 thin rects forming an L at every corner of the canvas.
    -- ----------------------------------------------------------------------
    local CORNER_LEN = 56
    local CORNER_THK = 2
    local CORNER_PAD = 22
    local A = UiItemBase.UiAnchorType
    local cornerSpec = {{
        name = "TL_H",
        v = A.TOP,
        h = A.LEFT,
        w = CORNER_LEN,
        t = CORNER_THK
    }, {
        name = "TL_V",
        v = A.TOP,
        h = A.LEFT,
        w = CORNER_THK,
        t = CORNER_LEN
    }, {
        name = "TR_H",
        v = A.TOP,
        h = A.RIGHT,
        w = CORNER_LEN,
        t = CORNER_THK
    }, {
        name = "TR_V",
        v = A.TOP,
        h = A.RIGHT,
        w = CORNER_THK,
        t = CORNER_LEN
    }, {
        name = "BL_H",
        v = A.BOTTOM,
        h = A.LEFT,
        w = CORNER_LEN,
        t = CORNER_THK
    }, {
        name = "BL_V",
        v = A.BOTTOM,
        h = A.LEFT,
        w = CORNER_THK,
        t = CORNER_LEN
    }, {
        name = "BR_H",
        v = A.BOTTOM,
        h = A.RIGHT,
        w = CORNER_LEN,
        t = CORNER_THK
    }, {
        name = "BR_V",
        v = A.BOTTOM,
        h = A.RIGHT,
        w = CORNER_THK,
        t = CORNER_LEN
    }}
    local corners = {}
    for i = 1, #cornerSpec do
        local rect = UiRectangle:new(host, "SLCorner_" .. cornerSpec[i].name)
        corners[i] = rect
        overlay:addWidget(rect)
    end

    -- ----------------------------------------------------------------------
    -- Top HUD: back chip (clickable), center label, commander chip
    -- ----------------------------------------------------------------------
    local hudBackChip = UiRectangle:new(host, "SLHudBackChip")
    local hudBackArrow = makeLabel(host, "SLHudBackArrow", "‹", 16, M.textBright,
        UiLabel.TextHorizontalAlignmentType.CENTER)
    local hudBackLabel = makeLabel(host, "SLHudBackLabel", "НАЗАД", 11, M.textBright,
        UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(hudBackChip)
    overlay:addWidget(hudBackArrow)
    overlay:addWidget(hudBackLabel)

    local hudCmdrChip = UiRectangle:new(host, "SLHudCmdrChip")
    local hudCmdrRank = makeLabel(host, "SLHudCmdrRank", "КОММОДОР", 10, M.textVeryDim,
        UiLabel.TextHorizontalAlignmentType.RIGHT)
    local hudCmdrName = makeLabel(host, "SLHudCmdrName", "«РЕЙВЕН»", 13, M.textBright,
        UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(hudCmdrChip)
    overlay:addWidget(hudCmdrRank)
    overlay:addWidget(hudCmdrName)

    -- ----------------------------------------------------------------------
    -- Title block
    -- ----------------------------------------------------------------------
    local titleEyebrow = makeLabel(host, "SLTitleEyebrow", "— КАМПАНИЯ · ЛИНИЯ КОТЭЛИН —", 13,
        M.textTitleAccent, UiLabel.TextHorizontalAlignmentType.CENTER)
    local titleMain = makeLabel(host, "SLTitleMain", "ВЫБОР УРОВНЯ", 72, M.textBright,
        UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(titleEyebrow)
    overlay:addWidget(titleMain)

    -- ----------------------------------------------------------------------
    -- Levels list: one row per level file. Rows without a level stay hidden and are reused when the list grows,
    -- because widgets can't be destroyed from lua.
    -- ----------------------------------------------------------------------
    local scrollList = UiScrollList:new(host, "SLScrollList")
    overlay:addWidget(scrollList)

    local emptyLabel = makeLabel(host, "SLEmptyLabel", "НЕТ СОХРАНЁННЫХ УРОВНЕЙ", 13, M.textDim,
        UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(emptyLabel)

    local levelsList = ParseLevelsList(_GetLevelsList(host))

    local rows = {}
    local setUpRowsCount = 0
    local isRowsDirty = true

    local function addRow(index)
        local prefix = "SLRow_" .. index
        local row = {
            bg = UiRectangle:new(host, prefix .. "_bg"),
            accentBar = UiRectangle:new(host, prefix .. "_accent"),
            indexLabel = makeLabel(host, prefix .. "_idx", string.format("%02d", index), 11, M.textVeryDim,
                UiLabel.TextHorizontalAlignmentType.CENTER),
            bracketL = makeLabel(host, prefix .. "_bl", "[", 22, M.textVeryDim,
                UiLabel.TextHorizontalAlignmentType.CENTER),
            nameLabel = makeLabel(host, prefix .. "_name", "", ROW_NAME_FONT_SIZE, M.textBright,
                UiLabel.TextHorizontalAlignmentType.LEFT),
            bracketR = makeLabel(host, prefix .. "_br", "]", 22, M.textVeryDim,
                UiLabel.TextHorizontalAlignmentType.CENTER),
            goLabel = makeLabel(host, prefix .. "_go", "НАЧАТЬ ›", 10, Combat.cyanGlow,
                UiLabel.TextHorizontalAlignmentType.RIGHT),
            isHovered = false
        }
        row.widgets = {row.bg, row.accentBar, row.indexLabel, row.bracketL, row.nameLabel, row.bracketR, row.goLabel}
        for _, widget in ipairs(row.widgets) do
            overlay:addWidget(widget)
        end
        rows[index] = row
    end

    for i = 1, #levelsList do
        addRow(i)
    end

    -- Hover look of a row, per the mockup's .lvl-row.is-hover
    local function applyRowHover(h, row, isHovered)
        if row.isHovered == isHovered then
            return
        end
        row.isHovered = isHovered
        if isHovered then
            row.bg:setColorHexValue(M.buttonHoverBgColor)
            row.bg:setOpacity(M.buttonHoverBgOpacity)
            row.bg:setBlurMix(M.buttonHoverBlurMix)
            row.indexLabel:setTextColorHexValue(Combat.cyanGlow)
            row.bracketL:setTextColorHexValue(Combat.cyan)
            row.bracketR:setTextColorHexValue(Combat.cyan)
            row.nameLabel:setTextColorHexValue(Combat.cyanGlow)
            row.goLabel:setOpacity(1.0)
            row.accentBar:startAnimation(h, "AccentReveal")
        else
            row.bg:setColorHexValue(M.buttonBgColor)
            row.bg:setOpacity(M.buttonBgOpacity)
            row.bg:setBlurMix(M.buttonBlurMix)
            row.indexLabel:setTextColorHexValue(M.textVeryDim)
            row.bracketL:setTextColorHexValue(M.textVeryDim)
            row.bracketR:setTextColorHexValue(M.textVeryDim)
            row.nameLabel:setTextColorHexValue(M.textBright)
            row.goLabel:setOpacity(0.0)
            row.accentBar:startAnimation(h, "AccentHide")
        end
    end

    local function startLevel(levelFileName)
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "CombatLevel", json.encode({
                lvl_name = levelFileName
            }))
    end

    local function setupRow(h, row, index)
        local cn = canvas.widgetName
        local rowHeight = ROW_PAD_V + row.nameLabel.safeLineHeight + ROW_PAD_V

        -- Plates are direct children of the scroll list (which stacks + scrolls them), so they carry NO anchors.
        row.bg:setParent(h, cn, scrollList.widgetName)
        row.bg:setWidth(ROW_WIDTH)
        row.bg:setHeight(rowHeight)
        row.bg:setColorHexValue(M.buttonBgColor)
        row.bg:setOpacity(M.buttonBgOpacity)
        row.bg:setBorderRadius(M.borderRadius)
        row.bg:setApplyBlur(true)
        row.bg:setBlurMix(M.buttonBlurMix)
        row.bg:setZOrder(Z_ROW_BG)
        row.bg:enableMouseInputReceiverBase(h)
        row.bg:setIfCanInterceptMouseInputEvent(true)
        -- a hidden row keeps its last position, so the callbacks ignore the rows without a level
        row.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            local isHovered = newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED
            applyRowHover(h, row, isHovered and levelsList[index] ~= nil)
        end)
        row.bg:subscribeOnMouseInputClickedCallback(function()
            if levelsList[index] ~= nil then
                startLevel(levelsList[index])
            end
        end)

        -- Left accent bar, hidden at rest and revealed on hover (.lvl-row::before)
        row.accentBar:setParent(h, cn, row.bg.widgetName)
        row.accentBar:setAnchor(A.LEFT, A.LEFT, row.bg.widgetName, 0)
        row.accentBar:setAnchor(A.TOP, A.TOP, row.bg.widgetName, 0)
        row.accentBar:setAnchor(A.BOTTOM, A.BOTTOM, row.bg.widgetName, 0)
        row.accentBar:setWidth(3)
        row.accentBar:setColorHexValue(Combat.cyan)
        row.accentBar:setOpacity(0.0)
        row.accentBar:setZOrder(Z_ROW_FG)
        row.accentBar:addAnimation(h, "AccentReveal", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.25,
            "Opacity", UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        row.accentBar:addAnimation(h, "AccentHide", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.25,
            "Opacity", UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)

        -- Index "01"
        applyLabelInit(row.indexLabel)
        row.indexLabel:setParent(h, cn, row.bg.widgetName)
        row.indexLabel:setWidth(ROW_INDEX_WIDTH)
        row.indexLabel:setHeight(row.indexLabel.safeLineHeight)
        row.indexLabel:setAnchor(A.LEFT, A.LEFT, row.bg.widgetName, ROW_PAD_H)
        row.indexLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, row.bg.widgetName)
        row.indexLabel:setZOrder(Z_ROW_FG)

        -- Left bracket "["
        applyLabelInit(row.bracketL)
        row.bracketL:setParent(h, cn, row.bg.widgetName)
        row.bracketL:setWidth(ROW_BRACKET_WIDTH)
        row.bracketL:setHeight(row.bracketL.safeLineHeight)
        row.bracketL:setAnchor(A.LEFT, A.RIGHT, row.indexLabel.widgetName, ROW_GAP)
        row.bracketL:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, row.bg.widgetName)
        row.bracketL:setZOrder(Z_ROW_FG)

        -- Level name, the text itself is set by refreshRows
        applyLabelInit(row.nameLabel)
        row.nameLabel:setParent(h, cn, row.bg.widgetName)
        row.nameLabel:setWidth(ROW_NAME_WIDTH)
        row.nameLabel:setHeight(row.nameLabel.safeLineHeight)
        row.nameLabel:setAnchor(A.LEFT, A.RIGHT, row.bracketL.widgetName, ROW_GAP)
        row.nameLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, row.bg.widgetName)
        row.nameLabel:setZOrder(Z_ROW_FG)

        -- "НАЧАТЬ ›", shown on hover only
        applyLabelInit(row.goLabel)
        row.goLabel:setParent(h, cn, row.bg.widgetName)
        row.goLabel:setWidth(ROW_GO_WIDTH)
        row.goLabel:setHeight(row.goLabel.safeLineHeight)
        row.goLabel:setAnchor(A.RIGHT, A.RIGHT, row.bg.widgetName, ROW_PAD_H)
        row.goLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, row.bg.widgetName)
        row.goLabel:setOpacity(0.0)
        row.goLabel:setZOrder(Z_ROW_FG)

        -- Right bracket "]"
        applyLabelInit(row.bracketR)
        row.bracketR:setParent(h, cn, row.bg.widgetName)
        row.bracketR:setWidth(ROW_BRACKET_WIDTH)
        row.bracketR:setHeight(row.bracketR.safeLineHeight)
        row.bracketR:setAnchor(A.RIGHT, A.LEFT, row.goLabel.widgetName, ROW_GAP)
        row.bracketR:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, row.bg.widgetName)
        row.bracketR:setZOrder(Z_ROW_FG)
    end

    -- applies levelsList to the rows
    local function refreshRows(h)
        scrollList:setScrollOffset(0) -- the old offset can be out of range for a shorter list
        forceWidgetVisibility(emptyLabel, #levelsList == 0)

        for i = 1, #rows do
            local row = rows[i]
            local levelFileName = levelsList[i]
            if levelFileName ~= nil then
                local displayName = getLevelDisplayName(levelFileName)
                row.nameLabel:setText(fitTextToWidth(h, displayName, ROW_NAME_FONT_SIZE, ROW_NAME_WIDTH - 4))
                forceWidgetVisibility(row.bg, true)
            else
                applyRowHover(h, row, false)
                forceWidgetVisibility(row.bg, false)
            end
        end
    end

    -- ----------------------------------------------------------------------
    -- Footer: version + key hints
    -- ----------------------------------------------------------------------
    local footerMeta = makeLabel(host, "SLFooterMeta", "v0.4.7-alpha    ·    BUILD 24281", 11, M.textVeryDim,
        UiLabel.TextHorizontalAlignmentType.LEFT)
    local footerKeys = makeLabel(host, "SLFooterKeys", "↑↓  ВЫБОР     ENTER  НАЧАТЬ     ESC  НАЗАД",
        11, M.textDim, UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(footerMeta)
    overlay:addWidget(footerKeys)

    -- ----------------------------------------------------------------------
    -- Layout — runs once all proxies are ready.
    -- ----------------------------------------------------------------------
    overlay:subscribeOnAllWidgetLuaProxiesReady(function(h, sender)
        print("SelectLevelOverlay:OnAllWidgetLuaProxiesReady: " .. tostring(sender.overlayName))

        local cn = canvas.widgetName

        -- ---------------- Scrim ----------------
        scrim:setParent(h, cn, cn)
        scrim:fill(cn)
        scrim:setColorHexValue(M.scrimColor)
        scrim:setOpacity(M.scrimOpacity)
        scrim:setZOrder(Z_SCRIM)

        -- ---------------- Corner brackets ----------------
        for i = 1, #corners do
            local rect = corners[i]
            local spec = cornerSpec[i]
            rect:setParent(h, cn, cn)
            rect:setColorHexValue(M.cornerColor)
            rect:setOpacity(0.75)
            rect:setZOrder(Z_CORNER)
            rect:setWidth(spec.w)
            rect:setHeight(spec.t)
            rect:setAnchor(spec.v, spec.v, cn, CORNER_PAD)
            rect:setAnchor(spec.h, spec.h, cn, CORNER_PAD)
        end

        -- ---------------- Top HUD ----------------
        local hudPadH = 28
        -- Back chip (clickable → close overlay).
        local backChipWidth = 120
        hudBackChip:setParent(h, cn, cn)
        hudBackChip:setColorHexValue(M.chipBgColor)
        hudBackChip:setOpacity(M.chipBgOpacity)
        hudBackChip:setBorderRadius(M.borderRadius)
        hudBackChip:setWidth(backChipWidth)
        hudBackChip:setHeight(36)
        hudBackChip:setAnchor(A.TOP, A.TOP, cn, 22)
        hudBackChip:setAnchor(A.LEFT, A.LEFT, cn, hudPadH)
        hudBackChip:setZOrder(Z_HUD)
        hudBackChip:enableMouseInputReceiverBase(h)
        hudBackChip:setIfCanInterceptMouseInputEvent(true)
        hudBackChip:subscribeOnMouseInputClickedCallback(function()
            UiOverlayManager:closeCurrentOverlay(h)
        end)
        hudBackChip:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                hudBackChip:setColorHexValue(M.buttonHoverBgColor)
                hudBackChip:setOpacity(M.buttonHoverBgOpacity)
                hudBackArrow:setTextColorHexValue(Combat.cyanGlow)
                hudBackLabel:setTextColorHexValue(Combat.cyanGlow)
            else
                hudBackChip:setColorHexValue(M.chipBgColor)
                hudBackChip:setOpacity(M.chipBgOpacity)
                hudBackArrow:setTextColorHexValue(M.textBright)
                hudBackLabel:setTextColorHexValue(M.textBright)
            end
        end)

        applyLabelInit(hudBackArrow)
        hudBackArrow:setParent(h, cn, hudBackChip.widgetName)
        hudBackArrow:setWidth(20)
        hudBackArrow:setHeight(hudBackArrow.safeLineHeight)
        hudBackArrow:setAnchor(A.LEFT, A.LEFT, hudBackChip.widgetName, 12)
        hudBackArrow:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, hudBackChip.widgetName)
        hudBackArrow:setZOrder(Z_HUD + 1)

        applyLabelInit(hudBackLabel)
        hudBackLabel:setParent(h, cn, hudBackChip.widgetName)
        hudBackLabel:setWidth(backChipWidth - 38)
        hudBackLabel:setHeight(hudBackLabel.safeLineHeight)
        hudBackLabel:setAnchor(A.LEFT, A.RIGHT, hudBackArrow.widgetName, 6)
        hudBackLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, hudBackChip.widgetName)
        hudBackLabel:setZOrder(Z_HUD + 1)

        -- Commander chip (same as main menu)
        local cmdrWidth = 168
        local cmdrPadTop = 6
        local cmdrGap = 2
        local cmdrPadBottom = 4
        local cmdrChipHeight = cmdrPadTop + hudCmdrRank.safeLineHeight + cmdrGap + hudCmdrName.safeLineHeight +
                                   cmdrPadBottom
        hudCmdrChip:setParent(h, cn, cn)
        hudCmdrChip:setColorHexValue(M.chipBgColor)
        hudCmdrChip:setOpacity(M.chipBgOpacity)
        hudCmdrChip:setBorderRadius(M.borderRadius)
        hudCmdrChip:setWidth(cmdrWidth)
        hudCmdrChip:setHeight(cmdrChipHeight)
        hudCmdrChip:setAnchor(A.TOP, A.TOP, cn, 18)
        hudCmdrChip:setAnchor(A.RIGHT, A.RIGHT, cn, hudPadH)
        hudCmdrChip:setZOrder(Z_HUD)

        applyLabelInit(hudCmdrRank)
        hudCmdrRank:setParent(h, cn, hudCmdrChip.widgetName)
        hudCmdrRank:setWidth(cmdrWidth - 20)
        hudCmdrRank:setHeight(hudCmdrRank.safeLineHeight)
        hudCmdrRank:setAnchor(A.TOP, A.TOP, hudCmdrChip.widgetName, cmdrPadTop)
        hudCmdrRank:setAnchor(A.RIGHT, A.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrRank:setZOrder(Z_HUD + 1)

        applyLabelInit(hudCmdrName)
        hudCmdrName:setParent(h, cn, hudCmdrChip.widgetName)
        hudCmdrName:setWidth(cmdrWidth - 20)
        hudCmdrName:setHeight(hudCmdrName.safeLineHeight)
        hudCmdrName:setAnchor(A.TOP, A.BOTTOM, hudCmdrRank.widgetName, cmdrGap)
        hudCmdrName:setAnchor(A.RIGHT, A.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrName:setZOrder(Z_HUD + 1)

        -- ---------------- Title block ----------------
        local titleTop = math.floor(windowHeight * 0.10)
        applyLabelInit(titleEyebrow)
        titleEyebrow:setParent(h, cn, cn)
        titleEyebrow:setWidth(700)
        titleEyebrow:setHeight(titleEyebrow.safeLineHeight)
        titleEyebrow:setAnchor(A.TOP, A.TOP, cn, titleTop)
        titleEyebrow:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        titleEyebrow:setZOrder(Z_TITLE)

        applyLabelInit(titleMain)
        titleMain:setParent(h, cn, cn)
        titleMain:setWidth(900)
        titleMain:setHeight(titleMain.safeLineHeight)
        titleMain:setAnchor(A.TOP, A.BOTTOM, titleEyebrow.widgetName, 4)
        titleMain:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        titleMain:setZOrder(Z_TITLE)
        titleMain:setTextGradientHexValues(UiLabel.TextGradientColorType.VERTICAL, Combat.cyanGlow, Combat.indigo)

        -- ---------------- Levels list ----------------
        -- .lvl-list-wrap: top 270px / bottom 90px of the 900px reference
        local listTop = math.floor(windowHeight * 0.30)
        local listHeight = windowHeight - listTop - math.floor(windowHeight * 0.10)
        scrollList:setParent(h, cn, cn)
        scrollList:setAnchor(A.TOP, A.TOP, cn, listTop)
        scrollList:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        scrollList:setWidth(LIST_WIDTH)
        scrollList:setHeight(listHeight)
        scrollList:setSpacing(ROW_SPACING)
        scrollList:setScrollSpeed(40)
        scrollList:setScrollbarSide(UiScrollList.ScrollbarSide.RIGHT)
        scrollList:setScrollbarThicknessPixels(6)
        scrollList:setScrollbarBackgroundColorHexValue(M.chipBgColor)
        scrollList:setScrollbarThumbColorHexValue(Combat.cyan)
        scrollList:setZOrder(Z_LIST)

        applyLabelInit(emptyLabel)
        emptyLabel:setParent(h, cn, cn)
        emptyLabel:setWidth(LIST_WIDTH)
        emptyLabel:setHeight(emptyLabel.safeLineHeight)
        emptyLabel:setAnchor(A.TOP, A.TOP, cn, listTop + 18)
        emptyLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        emptyLabel:setZOrder(Z_LIST)

        -- ---------------- Footer HUD ----------------
        applyLabelInit(footerMeta)
        footerMeta:setParent(h, cn, cn)
        footerMeta:setWidth(500)
        footerMeta:setHeight(footerMeta.safeLineHeight)
        footerMeta:setAnchor(A.BOTTOM, A.BOTTOM, cn, 22)
        footerMeta:setAnchor(A.LEFT, A.LEFT, cn, hudPadH)
        footerMeta:setZOrder(Z_HUD)

        applyLabelInit(footerKeys)
        footerKeys:setParent(h, cn, cn)
        footerKeys:setWidth(500)
        footerKeys:setHeight(footerKeys.safeLineHeight)
        footerKeys:setAnchor(A.BOTTOM, A.BOTTOM, cn, 22)
        footerKeys:setAnchor(A.RIGHT, A.RIGHT, cn, hudPadH)
        footerKeys:setZOrder(Z_HUD)
    end)

    -- The rows are changed only here, between updateFromReplicatorData and sendDataToReplicator. Values set in a game
    -- event handler can be overwritten by the replicator data before they are sent
    overlay.derivedUpdateCallback = function()
        if not isRowsDirty or not overlay.allWidgetLuaProxiesReady then
            return
        end
        for i = 1, #rows do
            for _, widget in ipairs(rows[i].widgets) do
                if not widget.luaProxyReady then
                    return
                end
            end
        end

        for i = setUpRowsCount + 1, #rows do
            setupRow(host, rows[i], i)
        end
        setUpRowsCount = #rows
        refreshRows(host)
        isRowsDirty = false
    end

    overlay.onGameEventTriggered = function(eventName, jsonArgs)
        if eventName == "LevelFilesListChanged" then
            levelsList = ParseLevelsList(jsonArgs)
            for i = #rows + 1, #levelsList do
                addRow(i)
            end
            isRowsDirty = true
        end
    end
    overlay.onEngineEventTriggered = function(eventName, jsonArgs)
    end
    overlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
    end

    return overlay
end

return SelectLevelOverlay
