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
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")

-- Galaxy-Defense-style main menu, modelled after UiDesign/menu-centered.jsx.
-- The space skybox lives in MainMenuLvl.lua; this overlay only paints a darkening
-- scrim plus HUD/title/menu/footer chrome on top of it. No full-screen background
-- panel is used, so the skybox is still visible behind every UI element.
MainMenuOverlay = {}

local FONT = "JetBrainsMono-VariableFont_wght"

local M = Styles.MainMenu

-- ZOrder layers (within the canvas — engine treats z as parent-local).
local Z_SCRIM = 1
local Z_CORNER = 5
local Z_HUD = 8
local Z_TITLE = 10
local Z_BTN_BG = 12
local Z_BTN_FG = 14

-- Safe-line-height helper lives in Ui/Common/uiTextSizing so settings, HUD
-- panels and any other overlay can share the same epsilon-free sizing rule.
local safeLineHeight = UiTextSizing.safeLineHeight

-- Helper: create a UiLabel pre-configured with text/size/color/alignment.
-- Returns the label so the caller can add it to the overlay and set anchors later.
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

function MainMenuOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    -- ----------------------------------------------------------------------
    -- Canvas + overlay
    -- ----------------------------------------------------------------------
    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "MainMenuCanvas")
    canvas:subscribeOnLuaProxyReady(function(h)
        _InitializeCanvasInputSystem(h, canvas.luaProxyId)
        canvas:addFadeInAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
                                  UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        canvas:addFadeOutAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
                                   UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local overlay = UiOverlay:createOverlay(host, "MainMenuOverlay", canvas)

    -- ----------------------------------------------------------------------
    -- Scrim — soft full-screen darkener so the skybox doesn't blow out the text.
    -- ----------------------------------------------------------------------
    local scrim = UiRectangle:new(host, "MainMenuScrim")
    overlay:addWidget(scrim)

    -- ----------------------------------------------------------------------
    -- Corner brackets — 8 thin rects forming an L at every corner of the canvas.
    -- ----------------------------------------------------------------------
    local CORNER_LEN = 56
    local CORNER_THK = 2
    local CORNER_PAD = 22
    local corners = {}
    local cornerSpec = {
        {
            name = "TL_H",
            v = UiItemBase.UiAnchorType.TOP,
            h = UiItemBase.UiAnchorType.LEFT,
            w = CORNER_LEN,
            t = CORNER_THK
        },
        {
            name = "TL_V",
            v = UiItemBase.UiAnchorType.TOP,
            h = UiItemBase.UiAnchorType.LEFT,
            w = CORNER_THK,
            t = CORNER_LEN
        },
        {
            name = "TR_H",
            v = UiItemBase.UiAnchorType.TOP,
            h = UiItemBase.UiAnchorType.RIGHT,
            w = CORNER_LEN,
            t = CORNER_THK
        },
        {
            name = "TR_V",
            v = UiItemBase.UiAnchorType.TOP,
            h = UiItemBase.UiAnchorType.RIGHT,
            w = CORNER_THK,
            t = CORNER_LEN
        },
        {
            name = "BL_H",
            v = UiItemBase.UiAnchorType.BOTTOM,
            h = UiItemBase.UiAnchorType.LEFT,
            w = CORNER_LEN,
            t = CORNER_THK
        },
        {
            name = "BL_V",
            v = UiItemBase.UiAnchorType.BOTTOM,
            h = UiItemBase.UiAnchorType.LEFT,
            w = CORNER_THK,
            t = CORNER_LEN
        },
        {
            name = "BR_H",
            v = UiItemBase.UiAnchorType.BOTTOM,
            h = UiItemBase.UiAnchorType.RIGHT,
            w = CORNER_LEN,
            t = CORNER_THK
        },
        {
            name = "BR_V",
            v = UiItemBase.UiAnchorType.BOTTOM,
            h = UiItemBase.UiAnchorType.RIGHT,
            w = CORNER_THK,
            t = CORNER_LEN
        }
    }
    for i = 1, #cornerSpec do
        local rect = UiRectangle:new(host, "MMCorner_" .. cornerSpec[i].name)
        corners[i] = rect
        overlay:addWidget(rect)
    end

    -- ----------------------------------------------------------------------
    -- Top HUD — left status chips, centered coordinate line, right commander chip
    -- ----------------------------------------------------------------------
    local hudTopHeight = 56
    local hudPadH = 28

    local hudStatusChip = UiRectangle:new(host, "MMHudStatusChip")
    local hudStatusDot = UiRectangle:new(host, "MMHudStatusDot")
    local hudStatusKey = makeLabel(host, "MMHudStatusKey", "СЕТЬ", 12, M.textVeryDim,
                                   UiLabel.TextHorizontalAlignmentType.LEFT)
    local hudStatusVal = makeLabel(host, "MMHudStatusVal", "СТАБИЛЬНА", 12, M.textBright,
                                   UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(hudStatusChip)
    overlay:addWidget(hudStatusDot)
    overlay:addWidget(hudStatusKey)
    overlay:addWidget(hudStatusVal)

    local hudNodeChip = UiRectangle:new(host, "MMHudNodeChip")
    local hudNodeKey = makeLabel(host, "MMHudNodeKey", "УЗЕЛ", 12, M.textVeryDim,
                                 UiLabel.TextHorizontalAlignmentType.LEFT)
    local hudNodeVal = makeLabel(host, "MMHudNodeVal", "КАЛИСТО-3", 12, M.textBright,
                                 UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(hudNodeChip)
    overlay:addWidget(hudNodeKey)
    overlay:addWidget(hudNodeVal)

    local hudCoord = makeLabel(host, "MMHudCoord", "⟨  RA 19h 35m  ·  DEC −12° 04′  ⟩", 12, M.textDim,
                               UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(hudCoord)

    local hudCmdrChip = UiRectangle:new(host, "MMHudCmdrChip")
    local hudCmdrRank = makeLabel(host, "MMHudCmdrRank", "КОММОДОР", 10, M.textVeryDim,
                                  UiLabel.TextHorizontalAlignmentType.RIGHT)
    local hudCmdrName = makeLabel(host, "MMHudCmdrName", "«РЕЙВЕН»", 13, M.textBright,
                                  UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(hudCmdrChip)
    overlay:addWidget(hudCmdrRank)
    overlay:addWidget(hudCmdrName)

    -- ----------------------------------------------------------------------
    -- Title block — eyebrow + main title + subtitle, centered horizontally
    -- ----------------------------------------------------------------------
    local titleEyebrow = makeLabel(host, "MMTitleEyebrow",
                                   "— ОПЕРАТИВНЫЙ ЦЕНТР · ЛИНИЯ КОТЭЛИН —", 13,
                                   M.textTitleAccent, UiLabel.TextHorizontalAlignmentType.CENTER)
    local titleMain = makeLabel(host, "MMTitleMain", "GALAXY DEFENSE", 72, M.textBright,
                                UiLabel.TextHorizontalAlignmentType.CENTER)
    local titleSub = makeLabel(host, "MMTitleSub", "— ОБОРОНА ОРБИТАЛЬНОГО РУБЕЖА —", 15,
                               M.textDim, UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(titleEyebrow)
    overlay:addWidget(titleMain)
    overlay:addWidget(titleSub)

    -- ----------------------------------------------------------------------
    -- Menu buttons — each is a translucent rect + an accent bar + 5 labels
    -- (index, [, mainLabel, sub, ], chevron). Hover/click handlers live on
    -- the rect itself via UiItemBase mouse input.
    -- ----------------------------------------------------------------------
    local MENU_ITEMS = {
        {
            id = "newgame",
            label = "НОВАЯ ИГРА",
            sub = "Начать новую кампанию",
            accent = M.accentPrimary,
            highlight = false,
            onClick = function()
                EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                          "CombatLevel")
            end
        }, {
            id = "editor",
            label = "РЕДАКТОР",
            sub = "Открыть редактор уровней",
            accent = M.accentSecondary,
            highlight = false,
            onClick = function()
                EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                          "EditorLevel")
            end
        }, {
            id = "settings",
            label = "НАСТРОЙКИ",
            sub = "Параметры и калибровка",
            accent = M.accentMuted,
            highlight = false,
            onClick = function() UiOverlayManager:openOverlay(host, "GameSettingsOverlay") end
        }, {
            id = "exit",
            label = "ВЫХОД",
            sub = "Покинуть командный центр",
            accent = M.accentMuted,
            highlight = false,
            onClick = function()
                EventsHelper:sendExitGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
            end
        }
    }

    local buttons = {}
    for i = 1, #MENU_ITEMS do
        local item = MENU_ITEMS[i]
        local b = {
            item = item,
            bg = UiRectangle:new(host, "MMBtn_" .. item.id .. "_bg"),
            accentBar = UiRectangle:new(host, "MMBtn_" .. item.id .. "_accent"),
            indexLabel = makeLabel(host, "MMBtn_" .. item.id .. "_idx", string.format("0%d", i), 16, M.textVeryDim,
                                   UiLabel.TextHorizontalAlignmentType.CENTER),
            bracketL = makeLabel(host, "MMBtn_" .. item.id .. "_bl", "[", 32, item.accent,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            mainLabel = makeLabel(host, "MMBtn_" .. item.id .. "_main", item.label, 26,
                                  item.highlight and M.textBright or item.accent,
                                  UiLabel.TextHorizontalAlignmentType.LEFT),
            subLabel = makeLabel(host, "MMBtn_" .. item.id .. "_sub", item.sub, 12, M.textDim,
                                 UiLabel.TextHorizontalAlignmentType.LEFT),
            bracketR = makeLabel(host, "MMBtn_" .. item.id .. "_br", "]", 32, item.accent,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            chevron = makeLabel(host, "MMBtn_" .. item.id .. "_chev", "›", 28, item.accent,
                                UiLabel.TextHorizontalAlignmentType.CENTER)
        }
        overlay:addWidget(b.bg)
        overlay:addWidget(b.accentBar)
        overlay:addWidget(b.indexLabel)
        overlay:addWidget(b.bracketL)
        overlay:addWidget(b.mainLabel)
        overlay:addWidget(b.subLabel)
        overlay:addWidget(b.bracketR)
        overlay:addWidget(b.chevron)
        buttons[i] = b
    end

    -- ----------------------------------------------------------------------
    -- Footer HUD — version meta on the left, key hints on the right
    -- ----------------------------------------------------------------------
    local footerMeta = makeLabel(host, "MMFooterMeta", "v0.4.7-alpha    ·    BUILD 24281    ·    © PHENGINE 2387",
                                 11, M.textVeryDim, UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(footerMeta)

    local footerKeys = makeLabel(host, "MMFooterKeys",
                                 "↑↓  ВЫБОР     ENTER  ПОДТВЕРДИТЬ     ESC  ВЫХОД", 11,
                                 M.textDim, UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(footerKeys)

    -- ----------------------------------------------------------------------
    -- Wire mouse input to the menu buttons (hover swaps colours, click acts).
    -- The mouse receiver has to be enabled after the lua proxy is ready —
    -- so we defer the wiring into the onAllWidgetLuaProxiesReady callback.
    -- ----------------------------------------------------------------------
    overlay:subscribeOnAllWidgetLuaProxiesReady(function(h, sender)
        print("MainMenuOverlay:OnAllWidgetLuaProxiesReady: " .. tostring(sender.overlayName))

        local canvasName = canvas.widgetName

        -- ---------------- Scrim ----------------
        scrim:setParent(h, canvasName, canvasName)
        scrim:fill(canvasName)
        scrim:setColorHexValue(M.scrimColor)
        scrim:setOpacity(M.scrimOpacity)
        scrim:setZOrder(Z_SCRIM)

        -- ---------------- Corner brackets ----------------
        for i = 1, #corners do
            local rect = corners[i]
            local spec = cornerSpec[i]
            rect:setParent(h, canvasName, canvasName)
            rect:setColorHexValue(M.cornerColor)
            rect:setOpacity(0.75)
            rect:setZOrder(Z_CORNER)
            rect:setWidth(spec.w)
            rect:setHeight(spec.t)
            rect:setAnchor(spec.v, spec.v, canvasName, CORNER_PAD)
            rect:setAnchor(spec.h, spec.h, canvasName, CORNER_PAD)
        end

        -- ---------------- Top HUD ----------------
        -- Status chip (network status with green dot)
        local statusChipWidth = 168
        hudStatusChip:setParent(h, canvasName, canvasName)
        hudStatusChip:setColorHexValue(M.chipBgColor)
        hudStatusChip:setOpacity(M.chipBgOpacity)
        hudStatusChip:setBorderRadius(M.borderRadius)
        hudStatusChip:setWidth(statusChipWidth)
        hudStatusChip:setHeight(hudTopHeight - 18)
        hudStatusChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, 22)
        hudStatusChip:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, canvasName, hudPadH)
        hudStatusChip:setZOrder(Z_HUD)

        hudStatusDot:setParent(h, canvasName, hudStatusChip.widgetName)
        hudStatusDot:setColorHexValue(0x22c55e) -- ok green
        hudStatusDot:setBorderRadius(4)
        hudStatusDot:setWidth(8)
        hudStatusDot:setHeight(8)
        hudStatusDot:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, hudStatusChip.widgetName, 12)
        hudStatusDot:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               hudStatusChip.widgetName)
        hudStatusDot:setZOrder(Z_HUD + 1)

        applyLabelInit(hudStatusKey)
        hudStatusKey:setParent(h, canvasName, hudStatusChip.widgetName)
        hudStatusKey:setWidth(40)
        hudStatusKey:setHeight(hudStatusKey.safeLineHeight)
        hudStatusKey:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudStatusDot.widgetName, 8)
        hudStatusKey:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               hudStatusChip.widgetName)
        hudStatusKey:setZOrder(Z_HUD + 1)

        applyLabelInit(hudStatusVal)
        hudStatusVal:setParent(h, canvasName, hudStatusChip.widgetName)
        hudStatusVal:setWidth(90)
        hudStatusVal:setHeight(hudStatusVal.safeLineHeight)
        hudStatusVal:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudStatusKey.widgetName, 6)
        hudStatusVal:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               hudStatusChip.widgetName)
        hudStatusVal:setZOrder(Z_HUD + 1)

        -- Node chip
        local nodeChipWidth = 168
        hudNodeChip:setParent(h, canvasName, canvasName)
        hudNodeChip:setColorHexValue(M.chipBgColor)
        hudNodeChip:setOpacity(M.chipBgOpacity)
        hudNodeChip:setBorderRadius(M.borderRadius)
        hudNodeChip:setWidth(nodeChipWidth)
        hudNodeChip:setHeight(hudTopHeight - 18)
        hudNodeChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, 22)
        hudNodeChip:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudStatusChip.widgetName, 10)
        hudNodeChip:setZOrder(Z_HUD)

        applyLabelInit(hudNodeKey)
        hudNodeKey:setParent(h, canvasName, hudNodeChip.widgetName)
        hudNodeKey:setWidth(44)
        hudNodeKey:setHeight(hudNodeKey.safeLineHeight)
        hudNodeKey:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, hudNodeChip.widgetName, 12)
        hudNodeKey:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                             hudNodeChip.widgetName)
        hudNodeKey:setZOrder(Z_HUD + 1)

        applyLabelInit(hudNodeVal)
        hudNodeVal:setParent(h, canvasName, hudNodeChip.widgetName)
        hudNodeVal:setWidth(100)
        hudNodeVal:setHeight(hudNodeVal.safeLineHeight)
        hudNodeVal:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudNodeKey.widgetName, 6)
        hudNodeVal:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                             hudNodeChip.widgetName)
        hudNodeVal:setZOrder(Z_HUD + 1)

        -- Coord text (centered)
        applyLabelInit(hudCoord)
        hudCoord:setParent(h, canvasName, canvasName)
        hudCoord:setWidth(360)
        hudCoord:setHeight(hudCoord.safeLineHeight)
        hudCoord:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, 28)
        hudCoord:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                           canvasName)
        hudCoord:setZOrder(Z_HUD + 1)

        -- Commander chip — sized to fit rank + name with safe line heights.
        local cmdrWidth = 168
        local cmdrPadTop = 6
        local cmdrGap = 2
        local cmdrPadBottom = 4
        local cmdrChipHeight = cmdrPadTop + hudCmdrRank.safeLineHeight + cmdrGap + hudCmdrName.safeLineHeight +
                                   cmdrPadBottom
        hudCmdrChip:setParent(h, canvasName, canvasName)
        hudCmdrChip:setColorHexValue(M.chipBgColor)
        hudCmdrChip:setOpacity(M.chipBgOpacity)
        hudCmdrChip:setBorderRadius(M.borderRadius)
        hudCmdrChip:setWidth(cmdrWidth)
        hudCmdrChip:setHeight(cmdrChipHeight)
        hudCmdrChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, 18)
        hudCmdrChip:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvasName, hudPadH)
        hudCmdrChip:setZOrder(Z_HUD)

        applyLabelInit(hudCmdrRank)
        hudCmdrRank:setParent(h, canvasName, hudCmdrChip.widgetName)
        hudCmdrRank:setWidth(cmdrWidth - 20)
        hudCmdrRank:setHeight(hudCmdrRank.safeLineHeight)
        hudCmdrRank:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, hudCmdrChip.widgetName,
                              cmdrPadTop)
        hudCmdrRank:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrRank:setZOrder(Z_HUD + 1)

        applyLabelInit(hudCmdrName)
        hudCmdrName:setParent(h, canvasName, hudCmdrChip.widgetName)
        hudCmdrName:setWidth(cmdrWidth - 20)
        hudCmdrName:setHeight(hudCmdrName.safeLineHeight)
        hudCmdrName:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, hudCmdrRank.widgetName,
                              cmdrGap)
        hudCmdrName:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrName:setZOrder(Z_HUD + 1)

        -- ---------------- Title block ----------------
        local titleAnchorTop = math.floor(windowHeight * 0.10)
        applyLabelInit(titleEyebrow)
        titleEyebrow:setParent(h, canvasName, canvasName)
        titleEyebrow:setWidth(700)
        titleEyebrow:setHeight(titleEyebrow.safeLineHeight)
        titleEyebrow:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, titleAnchorTop)
        titleEyebrow:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                               canvasName)
        titleEyebrow:setZOrder(Z_TITLE)

        applyLabelInit(titleMain)
        titleMain:setParent(h, canvasName, canvasName)
        titleMain:setWidth(1100)
        titleMain:setHeight(titleMain.safeLineHeight)
        titleMain:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, titleEyebrow.widgetName, 4)
        titleMain:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                            canvasName)
        titleMain:setZOrder(Z_TITLE)

        applyLabelInit(titleSub)
        titleSub:setParent(h, canvasName, canvasName)
        titleSub:setWidth(700)
        titleSub:setHeight(titleSub.safeLineHeight)
        titleSub:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, titleMain.widgetName, 4)
        titleSub:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                           canvasName)
        titleSub:setZOrder(Z_TITLE)

        -- ---------------- Menu buttons ----------------
        local BTN_WIDTH = 720
        local BTN_PAD_TOP = 14
        local BTN_PAD_BOTTOM = 12
        local BTN_INTER = 4 -- min gap between main and sub labels
        -- Derive button height from the safe-height labels it has to hold so we
        -- never trip FreeTypeTextMeshCreator's container/line-height check.
        local sampleBtn = buttons[1]
        local BTN_HEIGHT = BTN_PAD_TOP + sampleBtn.mainLabel.safeLineHeight + BTN_INTER +
                               sampleBtn.subLabel.safeLineHeight + BTN_PAD_BOTTOM
        local BTN_GAP = 14
        -- Vertically centred block, with a downward bias large enough that the
        -- 72pt title block above never collides with the first button.
        local totalH = #buttons * BTN_HEIGHT + (#buttons - 1) * BTN_GAP
        local menuTopOffset = math.floor((windowHeight - totalH) / 2) + math.floor(windowHeight * 0.10)

        for i = 1, #buttons do
            local b = buttons[i]
            local item = b.item
            local bgColor = item.highlight and M.buttonHoverBgColor or M.buttonBgColor
            local bgOpacity = item.highlight and M.buttonHoverBgOpacity or M.buttonBgOpacity

            -- Background plate
            b.bg:setParent(h, canvasName, canvasName)
            b.bg:setColorHexValue(bgColor)
            b.bg:setOpacity(bgOpacity)
            b.bg:setBorderRadius(M.borderRadius)
            b.bg:setWidth(BTN_WIDTH)
            b.bg:setHeight(BTN_HEIGHT)
            b.bg:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                           canvasName)
            if i == 1 then
                b.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvasName, menuTopOffset)
            else
                b.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                               buttons[i - 1].bg.widgetName, BTN_GAP)
            end
            b.bg:setZOrder(Z_BTN_BG)
            b.bg:enableMouseInputReceiverBase(h)
            b.bg:setIfCanInterceptMouseInputEvent(true)
            -- Frosted-glass tap on the plate (UiRectangle samples PostFx's
            -- Gaussian-blur RT and mixes it into the body colour). Highlighted
            -- buttons feel "denser" in the mockup, so they get a smaller mix.
            b.bg:setApplyBlur(true)
            b.bg:setBlurMix(item.highlight and M.buttonHoverBlurMix or M.buttonBlurMix)

            -- Left accent bar. Hidden at rest, revealed on hover (matches the
            -- mockup's `.menu-btn--centered::before { transform: scaleY(0) }`
            -- → `.is-hover/.is-highlight { scaleY(1) }`). The highlighted
            -- button keeps it visible permanently, just like .is-highlight.
            b.accentBar:setParent(h, canvasName, b.bg.widgetName)
            b.accentBar:setColorHexValue(item.accent)
            b.accentBar:setOpacity(item.highlight and 1.0 or 0.0)
            b.accentBar:setWidth(4)
            b.accentBar:setHeight(BTN_HEIGHT - 16)
            b.accentBar:setBorderRadius(2)
            b.accentBar:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, b.bg.widgetName, 10)
            b.accentBar:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                  b.bg.widgetName)
            b.accentBar:setZOrder(Z_BTN_FG)
            -- Fade animations driven from the hover callback. Highlighted
            -- buttons stay lit so they don't need the reveal/hide pair. The
            -- 0.58s LINEAR matches the mockup's `transition: transform 0.25s`
            -- closely enough that the bar feels animated rather than popped.
            if not item.highlight then
                b.accentBar:addAnimation(h, "AccentReveal", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
                                         0.58, "Opacity", UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
                b.accentBar:addAnimation(h, "AccentHide", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.58,
                                         "Opacity", UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
            end

            -- Index label "01"
            applyLabelInit(b.indexLabel)
            b.indexLabel:setParent(h, canvasName, b.bg.widgetName)
            b.indexLabel:setWidth(46)
            b.indexLabel:setHeight(b.indexLabel.safeLineHeight)
            b.indexLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, b.accentBar.widgetName,
                                   14)
            b.indexLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                   b.bg.widgetName)
            b.indexLabel:setZOrder(Z_BTN_FG)

            -- Left bracket "["
            applyLabelInit(b.bracketL)
            b.bracketL:setParent(h, canvasName, b.bg.widgetName)
            b.bracketL:setWidth(24)
            b.bracketL:setHeight(b.bracketL.safeLineHeight)
            b.bracketL:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, b.indexLabel.widgetName, 6)
            b.bracketL:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 b.bg.widgetName)
            b.bracketL:setZOrder(Z_BTN_FG)

            -- Main label
            applyLabelInit(b.mainLabel)
            b.mainLabel:setParent(h, canvasName, b.bg.widgetName)
            b.mainLabel:setWidth(420)
            b.mainLabel:setHeight(b.mainLabel.safeLineHeight)
            b.mainLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, b.bracketL.widgetName, 8)
            b.mainLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, b.bg.widgetName, BTN_PAD_TOP)
            b.mainLabel:setZOrder(Z_BTN_FG)

            -- Sub label
            applyLabelInit(b.subLabel)
            b.subLabel:setParent(h, canvasName, b.bg.widgetName)
            b.subLabel:setWidth(420)
            b.subLabel:setHeight(b.subLabel.safeLineHeight)
            b.subLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, b.bracketL.widgetName, 8)
            b.subLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, b.bg.widgetName,
                                 BTN_PAD_BOTTOM)
            b.subLabel:setZOrder(Z_BTN_FG)

            -- Right bracket "]" (anchored to the chevron's left)
            applyLabelInit(b.chevron)
            b.chevron:setParent(h, canvasName, b.bg.widgetName)
            b.chevron:setWidth(40)
            b.chevron:setHeight(b.chevron.safeLineHeight)
            b.chevron:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, b.bg.widgetName, 18)
            b.chevron:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                b.bg.widgetName)
            b.chevron:setZOrder(Z_BTN_FG)

            applyLabelInit(b.bracketR)
            b.bracketR:setParent(h, canvasName, b.bg.widgetName)
            b.bracketR:setWidth(24)
            b.bracketR:setHeight(b.bracketR.safeLineHeight)
            b.bracketR:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.LEFT, b.chevron.widgetName, 4)
            b.bracketR:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 b.bg.widgetName)
            b.bracketR:setZOrder(Z_BTN_FG)

            -- Hover: brighten the plate + bump the main label to bright text
            local baseBgColor = bgColor
            local baseBgOpacity = bgOpacity
            local baseMainColor = item.highlight and M.textBright or item.accent
            -- A highlighted plate stays at the "denser" blur mix on idle, so
            -- it doesn't visually drop when the hover moves away.
            local baseBlurMix = item.highlight and M.buttonHoverBlurMix or M.buttonBlurMix
            -- Highlighted buttons keep the accent bar lit even at rest, so the
            -- reveal/hide pair only fires on regular buttons.
            local animateAccent = not item.highlight
            b.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
                if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                    b.bg:setColorHexValue(M.buttonHoverBgColor)
                    b.bg:setOpacity(M.buttonHoverBgOpacity)
                    b.bg:setBlurMix(M.buttonHoverBlurMix)
                    b.mainLabel:setTextColorHexValue(M.textBright)
                    b.indexLabel:setTextColorHexValue(M.textDim)
                    if animateAccent then b.accentBar:startAnimation(h, "AccentReveal") end
                else
                    b.bg:setColorHexValue(baseBgColor)
                    b.bg:setOpacity(baseBgOpacity)
                    b.bg:setBlurMix(baseBlurMix)
                    b.mainLabel:setTextColorHexValue(baseMainColor)
                    b.indexLabel:setTextColorHexValue(M.textVeryDim)
                    if animateAccent then b.accentBar:startAnimation(h, "AccentHide") end
                end
            end)
            b.bg:subscribeOnMouseInputClickedCallback(item.onClick)
        end

        -- ---------------- Footer HUD ----------------
        applyLabelInit(footerMeta)
        footerMeta:setParent(h, canvasName, canvasName)
        footerMeta:setWidth(500)
        footerMeta:setHeight(footerMeta.safeLineHeight)
        footerMeta:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvasName, 22)
        footerMeta:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, canvasName, hudPadH)
        footerMeta:setZOrder(Z_HUD)

        applyLabelInit(footerKeys)
        footerKeys:setParent(h, canvasName, canvasName)
        footerKeys:setWidth(500)
        footerKeys:setHeight(footerKeys.safeLineHeight)
        footerKeys:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvasName, 22)
        footerKeys:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvasName, hudPadH)
        footerKeys:setZOrder(Z_HUD)
    end)

    overlay.onGameEventTriggered = function(eventName, jsonArgs) end
    overlay.onEngineEventTriggered = function(eventName, jsonArgs) end
    overlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return overlay
end

return MainMenuOverlay
