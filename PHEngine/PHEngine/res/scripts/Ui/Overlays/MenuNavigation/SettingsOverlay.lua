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

local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local UiSlider = require("Ui/Core/uiSlider")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")
local json = require("Ui/Core/3rdparty/json")

-- Settings screen modelled after UiDesign/menu-settings-centered.jsx. Uses the
-- same Galaxy Defense visual language as MainMenuOverlay: corner brackets,
-- HUD chips, bracketed rows, indigo accent on hover/active. Registered as
-- "GameSettingsOverlay" so the existing combat/editor pause→settings
-- transitions keep working — main menu opens the same overlay name.
SettingsOverlay = {}

local FONT = "JetBrainsMono-VariableFont_wght"
local M = Styles.MainMenu

-- ZOrder layers (local to canvas).
local Z_SCRIM = 1
local Z_CORNER = 5
local Z_HUD = 8
local Z_TITLE = 10
local Z_CHIP = 12
local Z_ROW_BG = 14
local Z_ROW_FG = 16
local Z_CTRL_TRACK = 18
local Z_CTRL_FG = 20

-- Safe-line-height helper lives in Ui/Common/uiTextSizing — shared with
-- MainMenuOverlay so any tweak to the multiplier (e.g. when switching font)
-- affects every overlay consistently.
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

-- Each category in the chip strip. Only "snd" has real controls hooked into
-- engine APIs (_GetMusicGain / _GetSoundGain + GeneralSystemSettingsEvents);
-- the other tabs display a placeholder row so the chip-switch behaviour is
-- still visible end-to-end without pretending we have settings we don't.
local CATEGORIES = {
    {id = "gfx", label = "ГРАФИКА"}, {id = "snd", label = "ЗВУК"},
    {id = "ctl", label = "УПРАВЛЕНИЕ"}, {id = "gpl", label = "ИГРОВОЙ ПРОЦЕСС"},
    {id = "ui", label = "ИНТЕРФЕЙС"}
}

function SettingsOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    -- ----------------------------------------------------------------------
    -- Canvas + overlay
    -- ----------------------------------------------------------------------
    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "PauseSettingsCanvas")
    canvas:subscribeOnLuaProxyReady(function(h)
        canvas:setCanvasZOrder(100)
        _InitializeCanvasInputSystem(h, canvas.luaProxyId)
        canvas:addFadeInAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
                                  UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        canvas:addFadeOutAnimation(h, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
                                   UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local overlay = UiOverlay:createOverlay(host, "GameSettingsOverlay", canvas)

    -- Tracking active category for chip switching. Initial = ЗВУК (only tab
    -- with real engine bindings, so users immediately see something useful).
    local activeCategoryId = "snd"

    -- ----------------------------------------------------------------------
    -- Scrim — darken the procedural space backdrop so text reads cleanly.
    -- ----------------------------------------------------------------------
    local scrim = UiRectangle:new(host, "SetScrim")
    overlay:addWidget(scrim)

    -- ----------------------------------------------------------------------
    -- Corner brackets — 8 thin rects forming L shapes in each corner.
    -- ----------------------------------------------------------------------
    local CORNER_LEN = 56
    local CORNER_THK = 2
    local CORNER_PAD = 22
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
    local corners = {}
    for i = 1, #cornerSpec do
        local rect = UiRectangle:new(host, "SetCorner_" .. cornerSpec[i].name)
        corners[i] = rect
        overlay:addWidget(rect)
    end

    -- ----------------------------------------------------------------------
    -- Top HUD: back chip (clickable), profile chip, center coord, commander chip
    -- ----------------------------------------------------------------------
    local hudBackChip = UiRectangle:new(host, "SetHudBackChip")
    local hudBackArrow = makeLabel(host, "SetHudBackArrow", "‹", 16, M.textBright,
                                   UiLabel.TextHorizontalAlignmentType.CENTER)
    local hudBackLabel = makeLabel(host, "SetHudBackLabel", "В ГЛАВНОЕ МЕНЮ", 11, M.textBright,
                                   UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(hudBackChip)
    overlay:addWidget(hudBackArrow)
    overlay:addWidget(hudBackLabel)

    local hudProfileChip = UiRectangle:new(host, "SetHudProfileChip")
    local hudProfileKey = makeLabel(host, "SetHudProfileKey", "ПРОФИЛЬ", 12, M.textVeryDim,
                                    UiLabel.TextHorizontalAlignmentType.LEFT)
    local hudProfileVal = makeLabel(host, "SetHudProfileVal", "RAVEN.01", 12, M.textBright,
                                    UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(hudProfileChip)
    overlay:addWidget(hudProfileKey)
    overlay:addWidget(hudProfileVal)

    local hudCenter = makeLabel(host, "SetHudCenter", "⟨  КОНФИГУРАЦИЯ СИСТЕМЫ  ⟩", 12,
                                M.textDim, UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(hudCenter)

    local hudCmdrChip = UiRectangle:new(host, "SetHudCmdrChip")
    local hudCmdrRank = makeLabel(host, "SetHudCmdrRank", "КОММОДОР", 10, M.textVeryDim,
                                  UiLabel.TextHorizontalAlignmentType.RIGHT)
    local hudCmdrName = makeLabel(host, "SetHudCmdrName", "«РЕЙВЕН»", 13, M.textBright,
                                  UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(hudCmdrChip)
    overlay:addWidget(hudCmdrRank)
    overlay:addWidget(hudCmdrName)

    -- ----------------------------------------------------------------------
    -- Title block
    -- ----------------------------------------------------------------------
    local titleEyebrow = makeLabel(host, "SetTitleEyebrow", "— КАЛИБРОВКА БОЕВОЙ СТАНЦИИ —",
                                   13, M.textTitleAccent, UiLabel.TextHorizontalAlignmentType.CENTER)
    local titleMain = makeLabel(host, "SetTitleMain", "НАСТРОЙКИ", 64, M.textBright,
                                UiLabel.TextHorizontalAlignmentType.CENTER)
    local titleSub = makeLabel(host, "SetTitleSub", "— SYSTEM · CONFIG · v0.4.7 —", 14, M.textDim,
                               UiLabel.TextHorizontalAlignmentType.CENTER)
    overlay:addWidget(titleEyebrow)
    overlay:addWidget(titleMain)
    overlay:addWidget(titleSub)

    -- ----------------------------------------------------------------------
    -- Category chips (horizontal strip). Each chip is bg + index + brackets + label.
    -- Click switches activeCategoryId; we restyle all chips and toggle row visibility.
    -- ----------------------------------------------------------------------
    local chips = {}
    for i = 1, #CATEGORIES do
        local cat = CATEGORIES[i]
        local chip = {
            id = cat.id,
            bg = UiRectangle:new(host, "SetChip_" .. cat.id .. "_bg"),
            accent = UiRectangle:new(host, "SetChip_" .. cat.id .. "_accent"),
            idxLabel = makeLabel(host, "SetChip_" .. cat.id .. "_idx", string.format("0%d", i), 11, M.textVeryDim,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            bracketL = makeLabel(host, "SetChip_" .. cat.id .. "_bl", "[", 18, M.accentSecondary,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            label = makeLabel(host, "SetChip_" .. cat.id .. "_lbl", cat.label, 13, M.textDim,
                              UiLabel.TextHorizontalAlignmentType.CENTER),
            bracketR = makeLabel(host, "SetChip_" .. cat.id .. "_br", "]", 18, M.accentSecondary,
                                 UiLabel.TextHorizontalAlignmentType.CENTER)
        }
        overlay:addWidget(chip.bg)
        overlay:addWidget(chip.accent)
        overlay:addWidget(chip.idxLabel)
        overlay:addWidget(chip.bracketL)
        overlay:addWidget(chip.label)
        overlay:addWidget(chip.bracketR)
        chips[i] = chip
    end

    -- ----------------------------------------------------------------------
    -- Settings rows. Each row has bg + accent bar + index + brackets +
    -- label/sub + control on the right. Built for two categories:
    --   • snd: musicSlider, soundSlider (fully wired via engine events).
    --   • everything else: a single placeholder row showing "В РАЗРАБОТКЕ".
    -- Rows for non-active categories live in the widget tree but stay hidden.
    -- ----------------------------------------------------------------------
    local function makeRowShell(rowId, idxNumber, label, sub)
        return {
            categoryId = nil, -- set by caller
            rowId = rowId,
            idxNumber = idxNumber,
            bg = UiRectangle:new(host, "SetRow_" .. rowId .. "_bg"),
            accent = UiRectangle:new(host, "SetRow_" .. rowId .. "_accent"),
            idxLabel = makeLabel(host, "SetRow_" .. rowId .. "_idx", string.format("0%d", idxNumber), 14, M.textVeryDim,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            bracketL = makeLabel(host, "SetRow_" .. rowId .. "_bl", "[", 22, M.accentSecondary,
                                 UiLabel.TextHorizontalAlignmentType.CENTER),
            label = makeLabel(host, "SetRow_" .. rowId .. "_lbl", label, 18, M.textBright,
                              UiLabel.TextHorizontalAlignmentType.LEFT),
            sub = makeLabel(host, "SetRow_" .. rowId .. "_sub", sub, 11, M.textDim,
                            UiLabel.TextHorizontalAlignmentType.LEFT),
            bracketR = makeLabel(host, "SetRow_" .. rowId .. "_br", "]", 22, M.accentSecondary,
                                 UiLabel.TextHorizontalAlignmentType.CENTER)
        }
    end

    local function registerRowShell(row)
        overlay:addWidget(row.bg)
        overlay:addWidget(row.accent)
        overlay:addWidget(row.idxLabel)
        overlay:addWidget(row.bracketL)
        overlay:addWidget(row.label)
        overlay:addWidget(row.sub)
        overlay:addWidget(row.bracketR)
    end

    -- Sound category rows.
    local rowMusic = makeRowShell("music", 1, "ГРОМКОСТЬ МУЗЫКИ",
                                  "Эмбиент-трек главного меню и боя")
    rowMusic.categoryId = "snd"
    rowMusic.slider = UiSlider:new(host, "SetRow_music_slider")
    rowMusic.valueLabel = makeLabel(host, "SetRow_music_val", "0%", 12, M.textBright,
                                    UiLabel.TextHorizontalAlignmentType.RIGHT)
    registerRowShell(rowMusic)
    overlay:addWidget(rowMusic.slider)
    overlay:addWidget(rowMusic.valueLabel)

    local rowSound = makeRowShell("sound", 2, "ГРОМКОСТЬ ЭФФЕКТОВ",
                                  "Выстрелы, взрывы, UI-фидбек")
    rowSound.categoryId = "snd"
    rowSound.slider = UiSlider:new(host, "SetRow_sound_slider")
    rowSound.valueLabel = makeLabel(host, "SetRow_sound_val", "0%", 12, M.textBright,
                                    UiLabel.TextHorizontalAlignmentType.RIGHT)
    registerRowShell(rowSound)
    overlay:addWidget(rowSound.slider)
    overlay:addWidget(rowSound.valueLabel)

    -- One generic placeholder row per non-snd category, so chip switching gives
    -- visible feedback even though there are no live bindings yet.
    local placeholderRows = {}
    for i = 1, #CATEGORIES do
        local cat = CATEGORIES[i]
        if cat.id ~= "snd" then
            local row = makeRowShell("ph_" .. cat.id, 1, cat.label,
                                     "Раздел в разработке. Скоро здесь появятся настройки.")
            row.categoryId = cat.id
            registerRowShell(row)
            placeholderRows[#placeholderRows + 1] = row
        end
    end

    -- ----------------------------------------------------------------------
    -- Actions: dirty indicator + Reset / Cancel / Apply.
    -- "Dirty" is tracked from baseline (the values present when overlay opens).
    -- ----------------------------------------------------------------------
    local dirtyDot = UiRectangle:new(host, "SetDirtyDot")
    local dirtyLabel = makeLabel(host, "SetDirtyLabel", "ВСЕ ИЗМЕНЕНИЯ ПРИМЕНЕНЫ", 12, M.textDim,
                                 UiLabel.TextHorizontalAlignmentType.LEFT)
    overlay:addWidget(dirtyDot)
    overlay:addWidget(dirtyLabel)

    local function makeActionButton(id, text, accent)
        return {
            bg = UiRectangle:new(host, "SetAction_" .. id .. "_bg"),
            label = makeLabel(host, "SetAction_" .. id .. "_lbl", text, 14, M.textBright,
                              UiLabel.TextHorizontalAlignmentType.CENTER),
            accent = accent
        }
    end

    local btnReset = makeActionButton("reset", "СБРОС", M.accentMuted)
    local btnCancel = makeActionButton("cancel", "ОТМЕНА", M.accentSecondary)
    local btnApply = makeActionButton("apply", "ПРИМЕНИТЬ", M.accentPrimary)
    for _, b in ipairs({btnReset, btnCancel, btnApply}) do
        overlay:addWidget(b.bg)
        overlay:addWidget(b.label)
    end

    -- ----------------------------------------------------------------------
    -- Footer: version + key hints
    -- ----------------------------------------------------------------------
    local footerMeta = makeLabel(host, "SetFooterMeta", "v0.4.7-alpha    ·    BUILD 24281    ·    © PHENGINE 2387",
                                 11, M.textVeryDim, UiLabel.TextHorizontalAlignmentType.LEFT)
    local footerKeys = makeLabel(host, "SetFooterKeys",
                                 "TAB  КАТЕГОРИЯ     ENTER  ПРИМЕНИТЬ     ESC  НАЗАД", 11,
                                 M.textDim, UiLabel.TextHorizontalAlignmentType.RIGHT)
    overlay:addWidget(footerMeta)
    overlay:addWidget(footerKeys)

    -- ----------------------------------------------------------------------
    -- Baseline values for the dirty flag + Reset behaviour. Refreshed
    -- whenever overlay opens (see onAllWidgetLuaProxiesReady below).
    -- ----------------------------------------------------------------------
    local baselineMusicGain = 0.0
    local baselineSoundGain = 0.0

    local function applyDirtyIndicator()
        local musicNow = rowMusic.slider:getSliderValue()
        local soundNow = rowSound.slider:getSliderValue()
        local musicDelta = math.abs(musicNow - baselineMusicGain)
        local soundDelta = math.abs(soundNow - baselineSoundGain)
        local isDirty = musicDelta > 0.001 or soundDelta > 0.001
        if isDirty then
            dirtyDot:setColorHexValue(M.accentPrimary)
            dirtyDot:setOpacity(1.0)
            dirtyLabel:setText("ИЗМЕНЕНИЯ НЕ СОХРАНЕНЫ")
            dirtyLabel:setTextColorHexValue(M.textBright)
        else
            dirtyDot:setColorHexValue(0x22c55e) -- ok green
            dirtyDot:setOpacity(0.85)
            dirtyLabel:setText("ВСЕ ИЗМЕНЕНИЯ ПРИМЕНЕНЫ")
            dirtyLabel:setTextColorHexValue(M.textDim)
        end
    end

    -- ----------------------------------------------------------------------
    -- Layout — runs once all proxies are ready.
    -- ----------------------------------------------------------------------
    overlay:subscribeOnAllWidgetLuaProxiesReady(function(h, sender)
        print("SettingsOverlay:OnAllWidgetLuaProxiesReady: " .. tostring(sender.overlayName))

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
        local backChipWidth = 200
        hudBackChip:setParent(h, cn, cn)
        hudBackChip:setColorHexValue(M.chipBgColor)
        hudBackChip:setOpacity(M.chipBgOpacity)
        hudBackChip:setBorderRadius(M.borderRadius)
        hudBackChip:setWidth(backChipWidth)
        hudBackChip:setHeight(36)
        hudBackChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, 22)
        hudBackChip:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cn, hudPadH)
        hudBackChip:setZOrder(Z_HUD)
        hudBackChip:enableMouseInputReceiverBase(h)
        hudBackChip:setIfCanInterceptMouseInputEvent(true)
        hudBackChip:subscribeOnMouseInputClickedCallback(function() UiOverlayManager:closeCurrentOverlay(h) end)
        hudBackChip:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                hudBackChip:setColorHexValue(M.buttonHoverBgColor)
                hudBackChip:setOpacity(M.buttonHoverBgOpacity)
            else
                hudBackChip:setColorHexValue(M.chipBgColor)
                hudBackChip:setOpacity(M.chipBgOpacity)
            end
        end)

        applyLabelInit(hudBackArrow)
        hudBackArrow:setParent(h, cn, hudBackChip.widgetName)
        hudBackArrow:setWidth(20)
        hudBackArrow:setHeight(hudBackArrow.safeLineHeight)
        hudBackArrow:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, hudBackChip.widgetName, 12)
        hudBackArrow:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               hudBackChip.widgetName)
        hudBackArrow:setZOrder(Z_HUD + 1)

        applyLabelInit(hudBackLabel)
        hudBackLabel:setParent(h, cn, hudBackChip.widgetName)
        hudBackLabel:setWidth(backChipWidth - 38)
        hudBackLabel:setHeight(hudBackLabel.safeLineHeight)
        hudBackLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudBackArrow.widgetName, 6)
        hudBackLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               hudBackChip.widgetName)
        hudBackLabel:setZOrder(Z_HUD + 1)

        -- Profile chip
        local profileChipWidth = 168
        hudProfileChip:setParent(h, cn, cn)
        hudProfileChip:setColorHexValue(M.chipBgColor)
        hudProfileChip:setOpacity(M.chipBgOpacity)
        hudProfileChip:setBorderRadius(M.borderRadius)
        hudProfileChip:setWidth(profileChipWidth)
        hudProfileChip:setHeight(36)
        hudProfileChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, 22)
        hudProfileChip:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudBackChip.widgetName, 10)
        hudProfileChip:setZOrder(Z_HUD)

        applyLabelInit(hudProfileKey)
        hudProfileKey:setParent(h, cn, hudProfileChip.widgetName)
        hudProfileKey:setWidth(70)
        hudProfileKey:setHeight(hudProfileKey.safeLineHeight)
        hudProfileKey:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, hudProfileChip.widgetName,
                                12)
        hudProfileKey:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                hudProfileChip.widgetName)
        hudProfileKey:setZOrder(Z_HUD + 1)

        applyLabelInit(hudProfileVal)
        hudProfileVal:setParent(h, cn, hudProfileChip.widgetName)
        hudProfileVal:setWidth(80)
        hudProfileVal:setHeight(hudProfileVal.safeLineHeight)
        hudProfileVal:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, hudProfileKey.widgetName, 4)
        hudProfileVal:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                hudProfileChip.widgetName)
        hudProfileVal:setZOrder(Z_HUD + 1)

        -- Center label
        applyLabelInit(hudCenter)
        hudCenter:setParent(h, cn, cn)
        hudCenter:setWidth(400)
        hudCenter:setHeight(hudCenter.safeLineHeight)
        hudCenter:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, 28)
        hudCenter:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER, cn)
        hudCenter:setZOrder(Z_HUD + 1)

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
        hudCmdrChip:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, 18)
        hudCmdrChip:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cn, hudPadH)
        hudCmdrChip:setZOrder(Z_HUD)

        applyLabelInit(hudCmdrRank)
        hudCmdrRank:setParent(h, cn, hudCmdrChip.widgetName)
        hudCmdrRank:setWidth(cmdrWidth - 20)
        hudCmdrRank:setHeight(hudCmdrRank.safeLineHeight)
        hudCmdrRank:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, hudCmdrChip.widgetName,
                              cmdrPadTop)
        hudCmdrRank:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrRank:setZOrder(Z_HUD + 1)

        applyLabelInit(hudCmdrName)
        hudCmdrName:setParent(h, cn, hudCmdrChip.widgetName)
        hudCmdrName:setWidth(cmdrWidth - 20)
        hudCmdrName:setHeight(hudCmdrName.safeLineHeight)
        hudCmdrName:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, hudCmdrRank.widgetName,
                              cmdrGap)
        hudCmdrName:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, hudCmdrChip.widgetName, 12)
        hudCmdrName:setZOrder(Z_HUD + 1)

        -- ---------------- Title block ----------------
        local titleTop = math.floor(windowHeight * 0.09)
        applyLabelInit(titleEyebrow)
        titleEyebrow:setParent(h, cn, cn)
        titleEyebrow:setWidth(700)
        titleEyebrow:setHeight(titleEyebrow.safeLineHeight)
        titleEyebrow:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, titleTop)
        titleEyebrow:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER, cn)
        titleEyebrow:setZOrder(Z_TITLE)

        applyLabelInit(titleMain)
        titleMain:setParent(h, cn, cn)
        titleMain:setWidth(900)
        titleMain:setHeight(titleMain.safeLineHeight)
        titleMain:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, titleEyebrow.widgetName, 4)
        titleMain:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER, cn)
        titleMain:setZOrder(Z_TITLE)

        applyLabelInit(titleSub)
        titleSub:setParent(h, cn, cn)
        titleSub:setWidth(600)
        titleSub:setHeight(titleSub.safeLineHeight)
        titleSub:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, titleMain.widgetName, 4)
        titleSub:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER, cn)
        titleSub:setZOrder(Z_TITLE)

        -- ---------------- Category chips ----------------
        local CHIP_HEIGHT = 36
        local CHIP_GAP = 12
        -- Approximate per-chip widths (longer labels get more room).
        -- Per-chip widths sized for the actual Cyrillic label widths at 13pt
        -- Lora. Label area inside a chip is roughly `cw - 72` (paddings +
        -- index + brackets), so a chip needs ~80-90px of slack on top of its
        -- text width to host the chrome without clipping.
        local chipWidths = {["gfx"] = 170, ["snd"] = 140, ["ctl"] = 220, ["gpl"] = 300, ["ui"] = 210}
        local totalChipsWidth = 0
        for i = 1, #chips do totalChipsWidth = totalChipsWidth + (chipWidths[chips[i].id] or 140) end
        totalChipsWidth = totalChipsWidth + CHIP_GAP * (#chips - 1)

        local chipsTop = math.floor(windowHeight * 0.30)
        local chipsLeft = math.floor((windowWidth - totalChipsWidth) / 2)

        local function styleChip(chip, isActive)
            if isActive then
                chip.bg:setColorHexValue(M.buttonHoverBgColor)
                chip.bg:setOpacity(M.buttonHoverBgOpacity)
                chip.accent:setOpacity(1.0)
                chip.label:setTextColorHexValue(M.textBright)
                chip.idxLabel:setTextColorHexValue(M.textDim)
                chip.bracketL:setTextColorHexValue(M.accentPrimary)
                chip.bracketR:setTextColorHexValue(M.accentPrimary)
            else
                chip.bg:setColorHexValue(M.chipBgColor)
                chip.bg:setOpacity(M.chipBgOpacity)
                chip.accent:setOpacity(0.0)
                chip.label:setTextColorHexValue(M.textDim)
                chip.idxLabel:setTextColorHexValue(M.textVeryDim)
                chip.bracketL:setTextColorHexValue(M.accentSecondary)
                chip.bracketR:setTextColorHexValue(M.accentSecondary)
            end
        end

        local function refreshRowVisibility()
            -- Sound rows on snd, placeholders on others.
            local isSnd = activeCategoryId == "snd"
            for _, row in ipairs({rowMusic, rowSound}) do
                row.bg:setIsVisible(isSnd)
                row.accent:setIsVisible(isSnd)
                row.idxLabel:setIsVisible(isSnd)
                row.bracketL:setIsVisible(isSnd)
                row.label:setIsVisible(isSnd)
                row.sub:setIsVisible(isSnd)
                row.bracketR:setIsVisible(isSnd)
                row.slider:setIsVisible(isSnd)
                row.valueLabel:setIsVisible(isSnd)
            end
            for _, prow in ipairs(placeholderRows) do
                local on = (prow.categoryId == activeCategoryId)
                prow.bg:setIsVisible(on)
                prow.accent:setIsVisible(on)
                prow.idxLabel:setIsVisible(on)
                prow.bracketL:setIsVisible(on)
                prow.label:setIsVisible(on)
                prow.sub:setIsVisible(on)
                prow.bracketR:setIsVisible(on)
            end
        end

        for i = 1, #chips do
            local chip = chips[i]
            local cw = chipWidths[chip.id] or 140
            chip.bg:setParent(h, cn, cn)
            chip.bg:setBorderRadius(M.borderRadius)
            chip.bg:setWidth(cw)
            chip.bg:setHeight(CHIP_HEIGHT)
            chip.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, chipsTop)
            if i == 1 then
                chip.bg:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cn, chipsLeft)
            else
                chip.bg:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                  chips[i - 1].bg.widgetName, CHIP_GAP)
            end
            chip.bg:setZOrder(Z_CHIP)
            chip.bg:enableMouseInputReceiverBase(h)
            chip.bg:setIfCanInterceptMouseInputEvent(true)

            -- Accent bar at top of chip (visible only when active).
            chip.accent:setParent(h, cn, chip.bg.widgetName)
            chip.accent:setColorHexValue(M.accentPrimary)
            chip.accent:setWidth(cw - 12)
            chip.accent:setHeight(2)
            chip.accent:setBorderRadius(1)
            chip.accent:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, chip.bg.widgetName, 4)
            chip.accent:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                  chip.bg.widgetName)
            chip.accent:setZOrder(Z_CHIP + 1)

            applyLabelInit(chip.idxLabel)
            chip.idxLabel:setParent(h, cn, chip.bg.widgetName)
            chip.idxLabel:setWidth(22)
            chip.idxLabel:setHeight(chip.idxLabel.safeLineHeight)
            chip.idxLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, chip.bg.widgetName, 8)
            chip.idxLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                    chip.bg.widgetName)
            chip.idxLabel:setZOrder(Z_CHIP + 1)

            applyLabelInit(chip.bracketL)
            chip.bracketL:setParent(h, cn, chip.bg.widgetName)
            chip.bracketL:setWidth(14)
            chip.bracketL:setHeight(chip.bracketL.safeLineHeight)
            chip.bracketL:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                    chip.idxLabel.widgetName, 2)
            chip.bracketL:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                    chip.bg.widgetName)
            chip.bracketL:setZOrder(Z_CHIP + 1)

            applyLabelInit(chip.bracketR)
            chip.bracketR:setParent(h, cn, chip.bg.widgetName)
            chip.bracketR:setWidth(14)
            chip.bracketR:setHeight(chip.bracketR.safeLineHeight)
            chip.bracketR:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, chip.bg.widgetName, 8)
            chip.bracketR:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                    chip.bg.widgetName)
            chip.bracketR:setZOrder(Z_CHIP + 1)

            applyLabelInit(chip.label)
            chip.label:setParent(h, cn, chip.bg.widgetName)
            chip.label:setWidth(cw - 60)
            chip.label:setHeight(chip.label.safeLineHeight)
            chip.label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, chip.bracketL.widgetName,
                                 2)
            chip.label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.LEFT, chip.bracketR.widgetName,
                                 2)
            chip.label:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 chip.bg.widgetName)
            chip.label:setZOrder(Z_CHIP + 1)

            local thisChip = chip
            chip.bg:subscribeOnMouseInputClickedCallback(function()
                if activeCategoryId == thisChip.id then return end
                activeCategoryId = thisChip.id
                for j = 1, #chips do styleChip(chips[j], chips[j].id == activeCategoryId) end
                refreshRowVisibility()
            end)
            chip.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
                local isActive = (activeCategoryId == thisChip.id)
                if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and not isActive then
                    thisChip.bg:setColorHexValue(M.buttonHoverBgColor)
                    thisChip.bg:setOpacity(0.7)
                    thisChip.label:setTextColorHexValue(M.textBright)
                else
                    styleChip(thisChip, isActive)
                end
            end)

            styleChip(chip, chip.id == activeCategoryId)
        end

        -- ---------------- Settings rows ----------------
        local ROW_WIDTH = 1000
        local ROW_HEIGHT = 72
        local ROW_GAP = 12
        local rowsTop = math.floor(windowHeight * 0.40)
        local rowsLeft = math.floor((windowWidth - ROW_WIDTH) / 2)

        local function layoutRowShell(row, topOffset)
            row.bg:setParent(h, cn, cn)
            row.bg:setColorHexValue(M.buttonBgColor)
            row.bg:setOpacity(M.buttonBgOpacity)
            row.bg:setBorderRadius(M.borderRadius)
            row.bg:setWidth(ROW_WIDTH)
            row.bg:setHeight(ROW_HEIGHT)
            row.bg:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cn, rowsLeft)
            row.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cn, topOffset)
            row.bg:setZOrder(Z_ROW_BG)
            row.bg:setApplyBlur(true)
            row.bg:setBlurMix(M.buttonBlurMix)

            row.accent:setParent(h, cn, row.bg.widgetName)
            row.accent:setColorHexValue(M.accentSecondary)
            row.accent:setOpacity(0.85)
            row.accent:setWidth(3)
            row.accent:setHeight(ROW_HEIGHT - 18)
            row.accent:setBorderRadius(2)
            row.accent:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, row.bg.widgetName, 10)
            row.accent:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 row.bg.widgetName)
            row.accent:setZOrder(Z_ROW_FG)

            applyLabelInit(row.idxLabel)
            row.idxLabel:setParent(h, cn, row.bg.widgetName)
            row.idxLabel:setWidth(40)
            row.idxLabel:setHeight(row.idxLabel.safeLineHeight)
            row.idxLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.accent.widgetName,
                                   12)
            row.idxLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                   row.bg.widgetName)
            row.idxLabel:setZOrder(Z_ROW_FG)

            applyLabelInit(row.bracketL)
            row.bracketL:setParent(h, cn, row.bg.widgetName)
            row.bracketL:setWidth(18)
            row.bracketL:setHeight(row.bracketL.safeLineHeight)
            row.bracketL:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.idxLabel.widgetName,
                                   4)
            row.bracketL:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                   row.bg.widgetName)
            row.bracketL:setZOrder(Z_ROW_FG)

            applyLabelInit(row.label)
            row.label:setParent(h, cn, row.bg.widgetName)
            row.label:setWidth(320)
            row.label:setHeight(row.label.safeLineHeight)
            row.label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.bracketL.widgetName, 6)
            row.label:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, row.bg.widgetName, 12)
            row.label:setZOrder(Z_ROW_FG)

            applyLabelInit(row.sub)
            row.sub:setParent(h, cn, row.bg.widgetName)
            row.sub:setWidth(320)
            row.sub:setHeight(row.sub.safeLineHeight)
            row.sub:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.bracketL.widgetName, 6)
            row.sub:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, row.bg.widgetName, 12)
            row.sub:setZOrder(Z_ROW_FG)

            applyLabelInit(row.bracketR)
            row.bracketR:setParent(h, cn, row.bg.widgetName)
            row.bracketR:setWidth(18)
            row.bracketR:setHeight(row.bracketR.safeLineHeight)
            row.bracketR:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.label.widgetName, 4)
            row.bracketR:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                   row.bg.widgetName)
            row.bracketR:setZOrder(Z_ROW_FG)
        end

        local function layoutSliderRow(row, topOffset, gainGetter, settingsType)
            layoutRowShell(row, topOffset)

            -- Slider lives in the right half of the row, with a numeric readout on the far right.
            row.slider:setParent(h, cn, row.bg.widgetName)
            row.slider:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, row.bracketR.widgetName,
                                 28)
            row.slider:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, row.bg.widgetName, 110)
            row.slider:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 row.bg.widgetName)
            row.slider:setHeight(28)
            row.slider:setZOrder(Z_CTRL_TRACK)
            local initialGain = gainGetter(host)
            row.slider:setSliderValue(initialGain)
            row.slider:setMaxSliderValue(1.0)
            row.slider:setMinSliderValue(0.0)
            row.slider:setSliderStep(0.05)
            row.slider:setSliderThicknessPixels(6.0)
            row.slider:setBlobThicknessPixels(18.0)
            row.slider:setSliderType(UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL)
            row.slider:setBlobColorHexValue(M.accentPrimary)
            row.slider:setSliderColorHexValue(0x1c2738)
            row.slider:enableSliderMouseInputReceiver(h)

            applyLabelInit(row.valueLabel)
            row.valueLabel:setParent(h, cn, row.bg.widgetName)
            row.valueLabel:setWidth(90)
            row.valueLabel:setHeight(row.valueLabel.safeLineHeight)
            row.valueLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, row.bg.widgetName, 18)
            row.valueLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                     row.bg.widgetName)
            row.valueLabel:setZOrder(Z_CTRL_FG)
            row.valueLabel:setText(tostring(math.floor(initialGain * 100 + 0.5)) .. "%")

            row.slider:subscribeOnSliderValueChangedCallback(function(newValue)
                local clamped = math.max(0.0, math.min(newValue, 1.0))
                row.valueLabel:setText(tostring(math.floor(clamped * 100 + 0.5)) .. "%")
                EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                          "GeneralSystemSettingsEvents",
                                                          json.encode({action = settingsType, gain = clamped}))
                applyDirtyIndicator()
            end)
        end

        -- Snd rows
        layoutSliderRow(rowMusic, rowsTop, _GetMusicGain, "change_music")
        layoutSliderRow(rowSound, rowsTop + ROW_HEIGHT + ROW_GAP, _GetSoundGain, "change_sound")
        baselineMusicGain = _GetMusicGain(host)
        baselineSoundGain = _GetSoundGain(host)

        -- Placeholder rows: one per non-snd category, all stacked at the same position
        -- (only one is visible at a time per refreshRowVisibility).
        for _, prow in ipairs(placeholderRows) do layoutRowShell(prow, rowsTop) end

        refreshRowVisibility()
        applyDirtyIndicator()

        -- ---------------- Actions ----------------
        local actionsBottom = math.floor(windowHeight * 0.10)
        local actionBtnHeight = 44
        local actionBtnWidth = 158
        local actionGap = 14

        -- Dirty indicator on the left.
        local dotSize = 10
        dirtyDot:setParent(h, cn, cn)
        dirtyDot:setBorderRadius(dotSize / 2)
        dirtyDot:setWidth(dotSize)
        dirtyDot:setHeight(dotSize)
        dirtyDot:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cn, rowsLeft + 2)
        dirtyDot:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cn,
                           actionsBottom + (actionBtnHeight - dotSize) / 2 + 1)
        dirtyDot:setZOrder(Z_HUD)

        applyLabelInit(dirtyLabel)
        dirtyLabel:setParent(h, cn, cn)
        dirtyLabel:setWidth(360)
        dirtyLabel:setHeight(dirtyLabel.safeLineHeight)
        dirtyLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, dirtyDot.widgetName, 10)
        dirtyLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cn,
                             actionsBottom + (actionBtnHeight - dirtyLabel.safeLineHeight) / 2)
        dirtyLabel:setZOrder(Z_HUD)

        -- Reset / Cancel / Apply on the right.
        local function layoutActionButton(btn, anchorRightRef, anchorMargin, onClick, primary)
            btn.bg:setParent(h, cn, cn)
            btn.bg:setBorderRadius(M.borderRadius)
            btn.bg:setWidth(actionBtnWidth)
            btn.bg:setHeight(actionBtnHeight)
            btn.bg:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cn, actionsBottom)
            if anchorRightRef == cn then
                btn.bg:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cn,
                                 rowsLeft + (windowWidth - ROW_WIDTH - 2 * rowsLeft))
                btn.bg:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cn, rowsLeft)
            else
                btn.bg:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.LEFT, anchorRightRef,
                                 anchorMargin)
            end
            local idleColor = primary and M.buttonHoverBgColor or M.buttonBgColor
            local idleOpacity = primary and M.buttonHoverBgOpacity or M.buttonBgOpacity
            btn.bg:setColorHexValue(idleColor)
            btn.bg:setOpacity(idleOpacity)
            btn.bg:setZOrder(Z_HUD)
            btn.bg:enableMouseInputReceiverBase(h)
            btn.bg:setIfCanInterceptMouseInputEvent(true)
            btn.bg:setApplyBlur(true)
            btn.bg:setBlurMix(primary and M.buttonHoverBlurMix or M.buttonBlurMix)
            btn.bg:subscribeOnMouseInputClickedCallback(onClick)
            btn.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
                if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                    btn.bg:setColorHexValue(M.buttonHoverBgColor)
                    btn.bg:setOpacity(M.buttonHoverBgOpacity)
                    btn.bg:setBlurMix(M.buttonHoverBlurMix)
                else
                    btn.bg:setColorHexValue(idleColor)
                    btn.bg:setOpacity(idleOpacity)
                    btn.bg:setBlurMix(primary and M.buttonHoverBlurMix or M.buttonBlurMix)
                end
            end)

            applyLabelInit(btn.label)
            btn.label:setParent(h, cn, btn.bg.widgetName)
            btn.label:setWidth(actionBtnWidth - 16)
            btn.label:setHeight(btn.label.safeLineHeight)
            btn.label:setTextColorHexValue(btn.accent)
            btn.label:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                btn.bg.widgetName)
            btn.label:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                btn.bg.widgetName)
            btn.label:setZOrder(Z_HUD + 1)
        end

        -- Apply (rightmost, primary). Persists current slider values as the new baseline
        -- and closes the overlay.
        layoutActionButton(btnApply, cn, 0, function()
            baselineMusicGain = rowMusic.slider:getSliderValue()
            baselineSoundGain = rowSound.slider:getSliderValue()
            applyDirtyIndicator()
            UiOverlayManager:closeCurrentOverlay(h)
        end, true)

        -- Cancel — restore the baseline (revert any unsaved tweaks) and close.
        layoutActionButton(btnCancel, btnApply.bg.widgetName, actionGap, function()
            rowMusic.slider:setSliderValue(baselineMusicGain)
            rowSound.slider:setSliderValue(baselineSoundGain)
            rowMusic.valueLabel:setText(tostring(math.floor(baselineMusicGain * 100 + 0.5)) .. "%")
            rowSound.valueLabel:setText(tostring(math.floor(baselineSoundGain * 100 + 0.5)) .. "%")
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "GeneralSystemSettingsEvents",
                                                      json.encode({action = "change_music", gain = baselineMusicGain}))
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "GeneralSystemSettingsEvents",
                                                      json.encode({action = "change_sound", gain = baselineSoundGain}))
            applyDirtyIndicator()
            UiOverlayManager:closeCurrentOverlay(h)
        end, false)

        -- Reset — set both to 1.0 (default in mockup is 100%).
        layoutActionButton(btnReset, btnCancel.bg.widgetName, actionGap, function()
            rowMusic.slider:setSliderValue(1.0)
            rowSound.slider:setSliderValue(1.0)
            rowMusic.valueLabel:setText("100%")
            rowSound.valueLabel:setText("100%")
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "GeneralSystemSettingsEvents",
                                                      json.encode({action = "change_music", gain = 1.0}))
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "GeneralSystemSettingsEvents",
                                                      json.encode({action = "change_sound", gain = 1.0}))
            applyDirtyIndicator()
        end, false)

        -- ---------------- Footer ----------------
        applyLabelInit(footerMeta)
        footerMeta:setParent(h, cn, cn)
        footerMeta:setWidth(500)
        footerMeta:setHeight(footerMeta.safeLineHeight)
        footerMeta:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cn, 22)
        footerMeta:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cn, hudPadH)
        footerMeta:setZOrder(Z_HUD)

        applyLabelInit(footerKeys)
        footerKeys:setParent(h, cn, cn)
        footerKeys:setWidth(500)
        footerKeys:setHeight(footerKeys.safeLineHeight)
        footerKeys:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cn, 22)
        footerKeys:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cn, hudPadH)
        footerKeys:setZOrder(Z_HUD)
    end)

    -- ----------------------------------------------------------------------
    -- Engine event plumbing — same channel the old overlay used so the
    -- GeneralSystemSettingsController keeps the sliders in sync with whatever
    -- actually persisted on disk.
    -- ----------------------------------------------------------------------
    overlay.onGameEventTriggered = function(eventName, jsonArgs) end
    overlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "GeneralSystemSettingsChanged" == eventName and overlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string", debug.traceback())
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["settings_type"] ~= nil then
                local settingsType = tostring(parsedJson["settings_type"])
                local action = tostring(parsedJson["action"])
                if "change_value" == action then
                    local gain = tonumber(parsedJson["gain"])
                    if gain ~= nil then
                        if "sound" == settingsType then
                            rowSound.slider:setSliderValue(gain)
                            rowSound.valueLabel:setText(tostring(math.floor(gain * 100 + 0.5)) .. "%")
                        elseif "music" == settingsType then
                            rowMusic.slider:setSliderValue(gain)
                            rowMusic.valueLabel:setText(tostring(math.floor(gain * 100 + 0.5)) .. "%")
                        end
                    end
                end
            end
        end
    end
    overlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return overlay
end

return SettingsOverlay
