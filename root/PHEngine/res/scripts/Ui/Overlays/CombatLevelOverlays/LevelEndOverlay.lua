--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local LabelButton = require("Ui/Widgets/LabelButton")
local UiItemBase = require("Ui/Core/uiItemBase")
local EventsHelper = require("Ui/Core/eventsHelper")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")

-- Unified end-of-mission overlay (victory + defeat), modelled after UiMockup/menu-end-centered.jsx.
-- One overlay with a mode ("victory" | "defeat") set via overlay.setMode before it is opened; the mode
-- swaps the title/colours and the right-hand column (rewards + medals vs. analysis + saved progress).
-- All numeric values are mockup placeholders — the engine does not expose run stats to Lua yet.
LevelEndOverlay = {}

local FONT = "JetBrainsMono-VariableFont_wght"

local C = Styles.Combat

local STAT_ROW_COUNT = 8
local INFO_ROW_COUNT = 3
local MEDAL_COUNT = 3

local COLOR_ROW_BG = 0x0a1322
local COLOR_RULE = 0x16314f

-- tone -> value colour (mirrors .end-row--* in the CSS).
local TONE_COLOR = {
    cyan = C.cyanGlow,
    purple = C.lilac,
    ok = 0x86efac,
    ink = C.textDim,
    fail = C.dangerSoft,
    warn = 0xfbbf24,
    info = C.cyan
}

local VICTORY = {
    accent = C.cyanGlow,
    accentDim = C.cyan,
    titleStart = 0x90e0ef,
    titleEnd = 0xc084fc,
    title = "ПОБЕДА",
    eyebrow = "— СТАНЦИЯ УДЕРЖАНА · СЕКТОР ОЧИЩЕН —",
    subtitle = "МИССИЯ ЗАВЕРШЕНА · ВСЕ 30 ВОЛН ОТРАЖЕНЫ",
    center = "ИТОГИ ОПЕРАЦИИ · УСПЕХ",
    statsTitle = "01 · СТАТИСТИКА БОЯ",
    rightTitle = "02 · НАЧИСЛЕНО",
    bottomTitle = "03 · МЕДАЛИ ОПЕРАЦИИ",
    stats = {
        {k = "ВОЛН ОТРАЖЕНО", v = "30 / 30", tone = "cyan"},
        {k = "ВРАГОВ УНИЧТОЖЕНО", v = "1 247", tone = "cyan"},
        {k = "БАШЕН ПОСТРОЕНО", v = "18", tone = "cyan"},
        {k = "БАРЬЕРОВ", v = "9", tone = "purple"}, {k = "ТОЧНОСТЬ", v = "87.4 %", tone = "cyan"},
        {k = "ВРЕМЯ МИССИИ", v = "24:18", tone = "purple"}, {k = "ПРОПУЩЕНО", v = "0", tone = "ok"},
        {k = "ЦЕЛОСТНОСТЬ", v = "94 %", tone = "cyan"}
    },
    info = {
        {k = "КРИСТАЛЛЫ", v = "+ 4 280", tone = "purple"},
        {k = "ОПЫТ КОМАНДИРА", v = "+ 1 250 XP", tone = "cyan"},
        {k = "ОЧКИ ИССЛЕДОВАНИЙ", v = "+ 3", tone = "cyan"}
    },
    actions = {
        {label = "ПОВТОР МИССИИ", action = "restart", primary = false},
        {label = "В ГЛАВНОЕ МЕНЮ", action = "menu", primary = false},
        {label = "СЛЕД. МИССИЯ", action = "next", primary = true}
    }
}

local DEFEAT = {
    accent = C.dangerSoft,
    accentDim = C.danger,
    titleStart = 0xfda4af,
    titleEnd = 0xf43f5e,
    title = "ПОРАЖЕНИЕ",
    eyebrow = "— СТАНЦИЯ УНИЧТОЖЕНА · ЛИНИЯ ПРОРВАНА —",
    subtitle = "МИССИЯ ПРОВАЛЕНА · ВОЛНА 17 ИЗ 30",
    center = "ИТОГИ ОПЕРАЦИИ · ПРОВАЛ",
    statsTitle = "01 · ОТЧЁТ О ПОТЕРЯХ",
    rightTitle = "02 · АНАЛИЗ КОМАНДИРА",
    bottomTitle = "03 · ПРОГРЕСС СОХРАНЁН",
    stats = {
        {k = "ВОЛН ОТРАЖЕНО", v = "17 / 30", tone = "fail"},
        {k = "ВРАГОВ УНИЧТОЖЕНО", v = "684", tone = "ink"},
        {k = "БАШЕН ПОСТРОЕНО", v = "12", tone = "ink"}, {k = "БАРЬЕРОВ", v = "6", tone = "ink"},
        {k = "ТОЧНОСТЬ", v = "72.1 %", tone = "ink"},
        {k = "ВРЕМЯ МИССИИ", v = "13:42", tone = "ink"}, {k = "ПРОПУЩЕНО", v = "21", tone = "fail"},
        {k = "ЦЕЛОСТНОСТЬ", v = "0 %", tone = "fail"}
    },
    info = {
        {
            k = "КРИТ. УЯЗВИМОСТЬ",
            v = "Сектор B-2 без дальнобойного покрытия.",
            tone = "fail"
        }, {
            k = "ДИСБАЛАНС ЭКОНОМИКИ",
            v = "68% бюджета — в башни ближнего боя.",
            tone = "warn"
        },
        {
            k = "РЕКОМЕНДАЦИЯ",
            v = "Изучите ветку «Точные системы».",
            tone = "info"
        }
    },
    actions = {
        {label = "В ГЛАВНОЕ МЕНЮ", action = "menu", primary = false},
        {label = "ДЕРЕВО ТЕХ.", action = "menu", primary = false},
        {label = "ПОВТОРИТЬ", action = "restart", primary = true}
    }
}

-- Operation medals (victory only; static placeholders — no engine achievement data yet).
local MEDALS_DATA = {
    {name = "БЕЗ ПРОПУСКОВ", sub = "0 врагов прошли"},
    {name = "БЛИЦКРИГ", sub = "< 25:00"}, {name = "СНАЙПЕР", sub = "точность > 85%"}
}

local function clamp_to(v, lo, hi)
    if v < lo then return lo end
    if v > hi then return hi end
    return v
end

function LevelEndOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)
    local A = UiItemBase.UiAnchorType
    local HALIGN = UiLabel.TextHorizontalAlignmentType
    local VALIGN = UiLabel.TextVerticalAlignmentType
    local function lh(fontSize) return UiTextSizing.safeLineHeight(fontSize, FONT) end

    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "LevelEndCanvas")
    canvas:subscribeOnLuaProxyReady(function(h) _InitializeCanvasInputSystem(h, canvas.luaProxyId) end)
    local overlay = UiOverlay:createOverlay(host, "LevelEndOverlay", canvas)

    local currentMode = "defeat"

    -- ── Widgets ────────────────────────────────────────────────────────────────
    local scrim = UiRectangle:new(host, "LevelEndScrim")
    overlay:addWidget(scrim)

    local braces = {}
    for i = 1, 8 do
        braces[i] = UiRectangle:new(host, "LevelEndBrace" .. tostring(i))
        overlay:addWidget(braces[i])
    end

    local backButton = LabelButton:new(host, overlay, FONT, "LevelEndBack")
    overlay:addCompoundWidget(backButton)
    local centerLabel = UiLabel:new(host, FONT, "LevelEndCenter")
    overlay:addWidget(centerLabel)

    local eyebrowLabel = UiLabel:new(host, FONT, "LevelEndEyebrow")
    overlay:addWidget(eyebrowLabel)
    local titleLabel = UiLabel:new(host, FONT, "LevelEndTitle")
    overlay:addWidget(titleLabel)
    local subtitleLabel = UiLabel:new(host, FONT, "LevelEndSubtitle")
    overlay:addWidget(subtitleLabel)

    -- Left column: section header + stat rows.
    local statsHeaderLabel = UiLabel:new(host, FONT, "LevelEndStatsHdr")
    overlay:addWidget(statsHeaderLabel)
    local statsHeaderRail = UiRectangle:new(host, "LevelEndStatsRail")
    overlay:addWidget(statsHeaderRail)
    local statRows = {}
    for i = 1, STAT_ROW_COUNT do
        local row = {
            bg = UiRectangle:new(host, "LevelEndStatBg" .. i),
            indexLabel = UiLabel:new(host, FONT, "LevelEndStatIdx" .. i),
            keyLabel = UiLabel:new(host, FONT, "LevelEndStatKey" .. i),
            valueLabel = UiLabel:new(host, FONT, "LevelEndStatVal" .. i)
        }
        overlay:addWidget(row.bg)
        overlay:addWidget(row.indexLabel)
        overlay:addWidget(row.keyLabel)
        overlay:addWidget(row.valueLabel)
        statRows[i] = row
    end

    -- Right column section 02: header + info rows (rewards / analysis).
    local rightHeaderLabel = UiLabel:new(host, FONT, "LevelEndRightHdr")
    overlay:addWidget(rightHeaderLabel)
    local rightHeaderRail = UiRectangle:new(host, "LevelEndRightRail")
    overlay:addWidget(rightHeaderRail)
    local infoRows = {}
    for i = 1, INFO_ROW_COUNT do
        local row = {
            bg = UiRectangle:new(host, "LevelEndInfoBg" .. i),
            accent = UiRectangle:new(host, "LevelEndInfoAccent" .. i),
            keyLabel = UiLabel:new(host, FONT, "LevelEndInfoKey" .. i),
            valueLabel = UiLabel:new(host, FONT, "LevelEndInfoVal" .. i)
        }
        overlay:addWidget(row.bg)
        overlay:addWidget(row.accent)
        overlay:addWidget(row.keyLabel)
        overlay:addWidget(row.valueLabel)
        infoRows[i] = row
    end

    -- Right column section 03: header + medals (victory) OR progress bar (defeat).
    local bottomHeaderLabel = UiLabel:new(host, FONT, "LevelEndBottomHdr")
    overlay:addWidget(bottomHeaderLabel)
    local bottomHeaderRail = UiRectangle:new(host, "LevelEndBottomRail")
    overlay:addWidget(bottomHeaderRail)
    local medals = {}
    for i = 1, MEDAL_COUNT do
        local medal = {
            bg = UiRectangle:new(host, "LevelEndMedalBg" .. i),
            nameLabel = UiLabel:new(host, FONT, "LevelEndMedalName" .. i),
            subLabel = UiLabel:new(host, FONT, "LevelEndMedalSub" .. i)
        }
        overlay:addWidget(medal.bg)
        overlay:addWidget(medal.nameLabel)
        overlay:addWidget(medal.subLabel)
        medals[i] = medal
    end
    local progressBg = UiRectangle:new(host, "LevelEndProgressBg")
    overlay:addWidget(progressBg)
    local progressFill = UiRectangle:new(host, "LevelEndProgressFill")
    overlay:addWidget(progressFill)
    local progressLabel = UiLabel:new(host, FONT, "LevelEndProgressLabel")
    overlay:addWidget(progressLabel)

    -- Bottom actions.
    local actionButtons = {}
    for i = 1, 3 do
        actionButtons[i] = LabelButton:new(host, overlay, FONT, "LevelEndAction" .. i)
        overlay:addCompoundWidget(actionButtons[i])
    end

    local footerLabel = UiLabel:new(host, FONT, "LevelEndFooter")
    overlay:addWidget(footerLabel)

    -- ── Actions ────────────────────────────────────────────────────────────────
    local function doRestart()
        EventsHelper:sendRestartLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end
    local function doMainMenu()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "MainMenuLevel")
    end
    -- TODO: no next-level API yet — falls back to the main menu.
    local ACTION_FN = {restart = doRestart, menu = doMainMenu, next = doMainMenu}

    backButton:subscribeOnMouseInputClickedCallback(doMainMenu)
    backButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        backButton:setButtonColorHexValue(newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and
                                              C.chipHoverColor or C.chipColor)
    end)

    -- ── Mode application: swaps text, colours and the right column per mode ──────
    local function applyMode(mode)
        local M = (mode == "victory") and VICTORY or DEFEAT
        local isVictory = (mode == "victory")

        for i = 1, 8 do braces[i]:setColorHexValue(M.accent) end

        centerLabel:setText("[ " .. M.center .. " ]")
        centerLabel:setTextColorHexValue(M.accent)

        eyebrowLabel:setText(M.eyebrow)
        eyebrowLabel:setTextColorHexValue(M.accent)

        titleLabel:setText(M.title)
        titleLabel:setTextGradientHexValues(UiLabel.TextGradientColorType.VERTICAL, M.titleStart, M.titleEnd)

        subtitleLabel:setText(M.subtitle)
        subtitleLabel:setTextColorHexValue(isVictory and C.textDim or M.accent)

        statsHeaderLabel:setText(M.statsTitle)
        statsHeaderLabel:setTextColorHexValue(M.accentDim)
        statsHeaderRail:setColorHexValue(M.accentDim)

        for i = 1, STAT_ROW_COUNT do
            local row = statRows[i]
            local data = M.stats[i]
            row.indexLabel:setText(string.format("%02d", i))
            row.keyLabel:setText(data.k)
            row.valueLabel:setText(data.v)
            row.valueLabel:setTextColorHexValue(TONE_COLOR[data.tone] or C.textBright)
        end

        rightHeaderLabel:setText(M.rightTitle)
        rightHeaderLabel:setTextColorHexValue(M.accentDim)
        rightHeaderRail:setColorHexValue(M.accentDim)

        for i = 1, INFO_ROW_COUNT do
            local row = infoRows[i]
            local data = M.info[i]
            row.accent:setColorHexValue(TONE_COLOR[data.tone] or C.cyan)
            row.keyLabel:setText(data.k)
            row.valueLabel:setText(data.v)
            row.valueLabel:setTextColorHexValue(isVictory and (TONE_COLOR[data.tone] or C.cyanGlow) or C.textDim)
        end

        bottomHeaderLabel:setText(M.bottomTitle)
        bottomHeaderLabel:setTextColorHexValue(M.accentDim)
        bottomHeaderRail:setColorHexValue(M.accentDim)

        -- Section 03 content: medals for victory, the saved-progress bar for defeat.
        for i = 1, MEDAL_COUNT do
            local medal = medals[i]
            medal.bg:setIsVisible(isVictory)
            medal.nameLabel:setIsVisible(isVictory)
            medal.subLabel:setIsVisible(isVictory)
        end
        progressBg:setIsVisible(not isVictory)
        progressFill:setIsVisible(not isVictory)
        progressLabel:setIsVisible(not isVictory)

        for i = 1, 3 do
            local btn = actionButtons[i]
            local data = M.actions[i]
            btn.__action = ACTION_FN[data.action]
            btn.__baseColor = data.primary and M.accentDim or C.chipColor
            btn.__labelColor = data.primary and (isVictory and 0x061522 or 0x1a0608) or C.textBright
            btn:setLabelText(data.label)
            btn:setButtonColorHexValue(btn.__baseColor)
            btn:setLabelTextColorHexValue(btn.__labelColor)
        end
    end

    for i = 1, 3 do
        local btn = actionButtons[i]
        btn:subscribeOnMouseInputClickedCallback(function() if btn.__action then btn.__action() end end)
        btn:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            local hovered = (newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED)
            btn:setButtonColorHexValue(hovered and C.chipHoverColor or (btn.__baseColor or C.chipColor))
        end)
    end

    -- ── Layout ───────────────────────────────────────────────────────────────────
    overlay:subscribeOnAllWidgetLuaProxiesReady(function(h, sender)
        local cn = canvas.widgetName

        scrim:setParent(h, cn, cn)
        scrim:fill(cn)
        scrim:setColorHexValue(0x04060c)
        scrim:setOpacity(0.82)
        scrim:setZOrder(0)

        -- Corner braces.
        local armLen = math.floor(windowWidth * 0.022)
        local braceThick = 2
        local braceInset = 24
        local corners = {
            {h = A.LEFT, v = A.TOP}, {h = A.RIGHT, v = A.TOP}, {h = A.LEFT, v = A.BOTTOM}, {h = A.RIGHT, v = A.BOTTOM}
        }
        for c = 1, 4 do
            local corner = corners[c]
            local hArm = braces[(c - 1) * 2 + 1]
            local vArm = braces[(c - 1) * 2 + 2]
            for _, arm in ipairs({hArm, vArm}) do
                arm:setParent(h, cn, cn)
                arm:setAnchor(corner.h, corner.h, cn, braceInset)
                arm:setAnchor(corner.v, corner.v, cn, braceInset)
                arm:setZOrder(2)
            end
            hArm:setWidth(armLen)
            hArm:setHeight(braceThick)
            vArm:setWidth(braceThick)
            vArm:setHeight(armLen)
        end

        -- Top HUD: back chip (left) + center caption.
        backButton:setParent(h, cn, cn)
        backButton:setAnchor(A.LEFT, A.LEFT, cn, 28)
        backButton:setAnchor(A.TOP, A.TOP, cn, 24)
        backButton:setWidth(220)
        backButton:setHeight(34)
        backButton:setButtonColorHexValue(C.chipColor)
        backButton:setButtonBorderRadius(C.chipBorderRadius)
        backButton:setLabelText("‹  В ГЛАВНОЕ МЕНЮ")
        backButton:setLabelFontSize(11)
        backButton:setLabelTextColorHexValue(C.textBright)
        backButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
        backButton:setLabelTextVerticalAlignment(VALIGN.CENTER)

        centerLabel:setParent(h, cn, cn)
        centerLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        centerLabel:setAnchor(A.TOP, A.TOP, cn, 28)
        centerLabel:setWidth(460)
        centerLabel:setHeight(lh(12))
        centerLabel:setFontSize(12)
        centerLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        centerLabel:setTextVerticalAlignment(VALIGN.CENTER)
        centerLabel:setZOrder(2)

        -- Title block.
        local titleTop = math.floor(windowHeight * 0.085)
        local titleFontSize = clamp_to(math.floor(windowHeight * 0.066), 44, 76)
        eyebrowLabel:setParent(h, cn, cn)
        eyebrowLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        eyebrowLabel:setAnchor(A.TOP, A.TOP, cn, titleTop)
        eyebrowLabel:setWidth(760)
        eyebrowLabel:setHeight(lh(12))
        eyebrowLabel:setFontSize(12)
        eyebrowLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        eyebrowLabel:setTextVerticalAlignment(VALIGN.CENTER)
        eyebrowLabel:setZOrder(2)

        titleLabel:setParent(h, cn, cn)
        titleLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        titleLabel:setAnchor(A.TOP, A.BOTTOM, eyebrowLabel.widgetName, 6)
        titleLabel:setWidth(900)
        titleLabel:setHeight(lh(titleFontSize))
        titleLabel:setFontSize(titleFontSize)
        titleLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
        titleLabel:setZOrder(2)

        subtitleLabel:setParent(h, cn, cn)
        subtitleLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        subtitleLabel:setAnchor(A.TOP, A.BOTTOM, titleLabel.widgetName, 6)
        subtitleLabel:setWidth(760)
        subtitleLabel:setHeight(lh(13))
        subtitleLabel:setFontSize(13)
        subtitleLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        subtitleLabel:setTextVerticalAlignment(VALIGN.CENTER)
        subtitleLabel:setZOrder(2)

        -- Two-column grid.
        local gridWidth = math.min(math.floor(windowWidth * 0.72), 1240)
        local gridGap = 36
        local colWidth = math.floor((gridWidth - gridGap) / 2)
        local gridTop = math.floor(windowHeight * 0.30)
        local leftX = math.floor((windowWidth - gridWidth) / 2)
        local rightX = leftX + colWidth + gridGap

        local function layoutSectionHeader(label, rail, x, y, width)
            label:setParent(h, cn, cn)
            label:setAnchor(A.LEFT, A.LEFT, cn, x)
            label:setAnchor(A.TOP, A.TOP, cn, y)
            label:setWidth(math.floor(width * 0.55))
            label:setHeight(lh(11))
            label:setFontSize(11)
            label:setTextHorizontalAlignment(HALIGN.LEFT)
            label:setTextVerticalAlignment(VALIGN.CENTER)
            label:setZOrder(2)

            rail:setParent(h, cn, cn)
            rail:setAnchor(A.LEFT, A.RIGHT, label.widgetName, 10)
            rail:setAnchor(A.RIGHT, A.LEFT, cn, x + width)
            rail:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, label.widgetName)
            rail:setHeight(1)
            rail:setZOrder(2)
        end

        -- Left section: stats.
        layoutSectionHeader(statsHeaderLabel, statsHeaderRail, leftX, gridTop, colWidth)
        local statRowH = 34
        local statRowGap = 3
        local statRowsTop = gridTop + lh(11) + 10
        for i = 1, STAT_ROW_COUNT do
            local row = statRows[i]
            local y = statRowsTop + (i - 1) * (statRowH + statRowGap)
            row.bg:setParent(h, cn, cn)
            row.bg:setAnchor(A.LEFT, A.LEFT, cn, leftX)
            row.bg:setAnchor(A.TOP, A.TOP, cn, y)
            row.bg:setWidth(colWidth)
            row.bg:setHeight(statRowH)
            row.bg:setColorHexValue(COLOR_ROW_BG)
            row.bg:setBorderRadius(4)
            row.bg:setZOrder(2)
            local bgn = row.bg.widgetName

            row.indexLabel:setParent(h, cn, bgn)
            row.indexLabel:setAnchor(A.LEFT, A.LEFT, bgn, 12)
            row.indexLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, bgn)
            row.indexLabel:setWidth(28)
            row.indexLabel:setHeight(lh(11))
            row.indexLabel:setFontSize(11)
            row.indexLabel:setTextColorHexValue(C.textVeryDim)
            row.indexLabel:setTextHorizontalAlignment(HALIGN.LEFT)
            row.indexLabel:setTextVerticalAlignment(VALIGN.CENTER)
            row.indexLabel:setZOrder(3)

            row.keyLabel:setParent(h, cn, bgn)
            row.keyLabel:setAnchor(A.LEFT, A.LEFT, bgn, 46)
            row.keyLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, bgn)
            row.keyLabel:setWidth(colWidth - 160)
            row.keyLabel:setHeight(lh(12))
            row.keyLabel:setFontSize(12)
            row.keyLabel:setTextColorHexValue(C.textDim)
            row.keyLabel:setTextHorizontalAlignment(HALIGN.LEFT)
            row.keyLabel:setTextVerticalAlignment(VALIGN.CENTER)
            row.keyLabel:setZOrder(3)

            row.valueLabel:setParent(h, cn, bgn)
            row.valueLabel:setAnchor(A.RIGHT, A.RIGHT, bgn, 14)
            row.valueLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, bgn)
            row.valueLabel:setWidth(110)
            row.valueLabel:setHeight(lh(14))
            row.valueLabel:setFontSize(14)
            row.valueLabel:setTextHorizontalAlignment(HALIGN.RIGHT)
            row.valueLabel:setTextVerticalAlignment(VALIGN.CENTER)
            row.valueLabel:setZOrder(3)
        end

        -- Right section 02: info rows.
        layoutSectionHeader(rightHeaderLabel, rightHeaderRail, rightX, gridTop, colWidth)
        local infoRowH = 50
        local infoRowGap = 6
        local infoRowsTop = gridTop + lh(11) + 10
        for i = 1, INFO_ROW_COUNT do
            local row = infoRows[i]
            local y = infoRowsTop + (i - 1) * (infoRowH + infoRowGap)
            row.bg:setParent(h, cn, cn)
            row.bg:setAnchor(A.LEFT, A.LEFT, cn, rightX)
            row.bg:setAnchor(A.TOP, A.TOP, cn, y)
            row.bg:setWidth(colWidth)
            row.bg:setHeight(infoRowH)
            row.bg:setColorHexValue(COLOR_ROW_BG)
            row.bg:setBorderRadius(4)
            row.bg:setZOrder(2)
            local bgn = row.bg.widgetName

            row.accent:setParent(h, cn, bgn)
            row.accent:setAnchor(A.LEFT, A.LEFT, bgn, 0)
            row.accent:setAnchor(A.TOP, A.TOP, bgn, 0)
            row.accent:setAnchor(A.BOTTOM, A.BOTTOM, bgn, 0)
            row.accent:setWidth(3)
            row.accent:setZOrder(3)

            row.keyLabel:setParent(h, cn, bgn)
            row.keyLabel:setAnchor(A.LEFT, A.LEFT, bgn, 16)
            row.keyLabel:setAnchor(A.TOP, A.TOP, bgn, 8)
            row.keyLabel:setWidth(colWidth - 28)
            row.keyLabel:setHeight(lh(10))
            row.keyLabel:setFontSize(10)
            row.keyLabel:setTextColorHexValue(C.textVeryDim)
            row.keyLabel:setTextHorizontalAlignment(HALIGN.LEFT)
            row.keyLabel:setTextVerticalAlignment(VALIGN.CENTER)
            row.keyLabel:setZOrder(3)

            row.valueLabel:setParent(h, cn, bgn)
            row.valueLabel:setAnchor(A.LEFT, A.LEFT, bgn, 16)
            row.valueLabel:setAnchor(A.BOTTOM, A.BOTTOM, bgn, 8)
            row.valueLabel:setWidth(colWidth - 28)
            row.valueLabel:setHeight(lh(14))
            row.valueLabel:setFontSize(14)
            row.valueLabel:setTextHorizontalAlignment(HALIGN.LEFT)
            row.valueLabel:setTextVerticalAlignment(VALIGN.CENTER)
            row.valueLabel:setZOrder(3)
        end

        -- Right section 03: header + medals / progress.
        local bottomTop = infoRowsTop + INFO_ROW_COUNT * (infoRowH + infoRowGap) + 18
        layoutSectionHeader(bottomHeaderLabel, bottomHeaderRail, rightX, bottomTop, colWidth)
        local bottomContentTop = bottomTop + lh(11) + 12

        -- Medals (victory): 3 columns.
        local medalGap = 8
        local medalW = math.floor((colWidth - 2 * medalGap) / 3)
        local medalH = 86
        for i = 1, MEDAL_COUNT do
            local medal = medals[i]
            local mx = rightX + (i - 1) * (medalW + medalGap)
            medal.bg:setParent(h, cn, cn)
            medal.bg:setAnchor(A.LEFT, A.LEFT, cn, mx)
            medal.bg:setAnchor(A.TOP, A.TOP, cn, bottomContentTop)
            medal.bg:setWidth(medalW)
            medal.bg:setHeight(medalH)
            medal.bg:setColorHexValue(COLOR_ROW_BG)
            medal.bg:setBorderRadius(6)
            medal.bg:setZOrder(2)
            local mbn = medal.bg.widgetName

            medal.nameLabel:setParent(h, cn, mbn)
            medal.nameLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, mbn)
            medal.nameLabel:setAnchor(A.TOP, A.TOP, mbn, 40)
            medal.nameLabel:setWidth(medalW - 8)
            medal.nameLabel:setHeight(lh(10))
            medal.nameLabel:setFontSize(10)
            medal.nameLabel:setTextColorHexValue(C.cyanGlow)
            medal.nameLabel:setTextHorizontalAlignment(HALIGN.CENTER)
            medal.nameLabel:setTextVerticalAlignment(VALIGN.CENTER)
            medal.nameLabel:setText(MEDALS_DATA[i].name)
            medal.nameLabel:setZOrder(3)

            medal.subLabel:setParent(h, cn, mbn)
            medal.subLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, mbn)
            medal.subLabel:setAnchor(A.TOP, A.TOP, mbn, 58)
            medal.subLabel:setWidth(medalW - 8)
            medal.subLabel:setHeight(lh(9))
            medal.subLabel:setFontSize(9)
            medal.subLabel:setTextColorHexValue(C.textVeryDim)
            medal.subLabel:setTextHorizontalAlignment(HALIGN.CENTER)
            medal.subLabel:setTextVerticalAlignment(VALIGN.CENTER)
            medal.subLabel:setText(MEDALS_DATA[i].sub)
            medal.subLabel:setZOrder(3)
        end

        -- Progress bar (defeat).
        progressLabel:setParent(h, cn, cn)
        progressLabel:setAnchor(A.LEFT, A.LEFT, cn, rightX)
        progressLabel:setAnchor(A.TOP, A.TOP, cn, bottomContentTop)
        progressLabel:setWidth(colWidth)
        progressLabel:setHeight(lh(11))
        progressLabel:setFontSize(11)
        progressLabel:setTextColorHexValue(C.dangerSoft)
        progressLabel:setTextHorizontalAlignment(HALIGN.LEFT)
        progressLabel:setTextVerticalAlignment(VALIGN.CENTER)
        progressLabel:setText("ВОЛНА 17 / 30  ·  +280 КРИСТ.  ·  +120 XP")
        progressLabel:setZOrder(3)

        progressBg:setParent(h, cn, cn)
        progressBg:setAnchor(A.LEFT, A.LEFT, cn, rightX)
        progressBg:setAnchor(A.TOP, A.BOTTOM, progressLabel.widgetName, 10)
        progressBg:setWidth(colWidth)
        progressBg:setHeight(12)
        progressBg:setColorHexValue(0x05080e)
        progressBg:setBorderRadius(2)
        progressBg:setZOrder(2)

        progressFill:setParent(h, cn, progressBg.widgetName)
        progressFill:setAnchor(A.LEFT, A.LEFT, progressBg.widgetName, 0)
        progressFill:setAnchor(A.TOP, A.TOP, progressBg.widgetName, 0)
        progressFill:setAnchor(A.BOTTOM, A.BOTTOM, progressBg.widgetName, 0)
        progressFill:setWidth(math.floor(colWidth * (17 / 30)))
        progressFill:setColorHexValue(C.danger)
        progressFill:setBorderRadius(2)
        progressFill:setZOrder(3)

        -- Bottom actions: 3 buttons, centered.
        local actionW = 200
        local actionH = 46
        local actionGap = 12
        local actionsTotal = 3 * actionW + 2 * actionGap
        local actionsLeft = math.floor((windowWidth - actionsTotal) / 2)
        local actionsBottom = math.floor(windowHeight * 0.07)
        for i = 1, 3 do
            local btn = actionButtons[i]
            btn:setParent(h, cn, cn)
            btn:setWidth(actionW)
            btn:setHeight(actionH)
            btn:setAnchor(A.LEFT, A.LEFT, cn, actionsLeft + (i - 1) * (actionW + actionGap))
            btn:setAnchor(A.BOTTOM, A.BOTTOM, cn, actionsBottom)
            btn:setButtonBorderRadius(C.chipBorderRadius)
            btn:setLabelFontSize(12)
            btn:setLabelTextHorizontalAlignment(HALIGN.CENTER)
            btn:setLabelTextVerticalAlignment(VALIGN.CENTER)
        end

        footerLabel:setParent(h, cn, cn)
        footerLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        footerLabel:setAnchor(A.BOTTOM, A.BOTTOM, cn, 22)
        footerLabel:setWidth(760)
        footerLabel:setHeight(lh(10))
        footerLabel:setFontSize(10)
        footerLabel:setTextColorHexValue(C.textVeryDim)
        footerLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        footerLabel:setTextVerticalAlignment(VALIGN.CENTER)
        footerLabel:setText("v0.4.7-alpha  ·  BUILD 24281  ·  ОПЕРАЦИЯ «РУБЕЖ»")
        footerLabel:setZOrder(2)

        applyMode(currentMode)
    end)

    -- Called by the combat HUD just before opening the overlay to pick victory / defeat.
    overlay.setMode = function(h, mode)
        currentMode = (mode == "victory") and "victory" or "defeat"
        if overlay.allWidgetLuaProxiesReady then applyMode(currentMode) end
    end

    overlay.onGameEventTriggered = function(eventName, jsonArgs) end
    overlay.onEngineEventTriggered = function(eventName, jsonArgs) end
    overlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return overlay
end

return LevelEndOverlay
