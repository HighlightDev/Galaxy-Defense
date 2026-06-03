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
local EventsHelper = require("Ui/Core/eventsHelper")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local UiImage = require("Ui/Core/uiImage")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")

local FONT = "JetBrainsMono-VariableFont_wght"

PauseOverlay = {}

-- Menu items, mirroring UiMockup/menu-pause-centered.jsx. `accent` colours the left bar and label;
-- `actionId` selects the click behaviour wired below.
local function buildItems(Combat)
    return {
        {id = "resume", label = "ПРОДОЛЖИТЬ", sub = "Вернуться к обороне станции", accent = Combat.cyanGlow,
         icon = "play.png"},
        {id = "settings", label = "НАСТРОЙКИ", sub = "Графика · звук · управление", accent = Combat.textBright,
         icon = "gear.png"},
        {id = "tomenu", label = "ВЫЙТИ В ГЛАВНОЕ МЕНЮ", sub = "Прогресс волны будет сохранён",
         accent = Combat.dangerSoft, icon = "arrow-left.png"},
        {id = "quit", label = "ВЫЙТИ ИЗ ИГРЫ", sub = "Завершить сеанс", accent = Combat.dangerSoft,
         icon = "sign-out.png"}
    }
end

function PauseOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)
    local Combat = Styles.Combat

    local pauseMenuOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "PauseMenuCanvas")
    pauseMenuOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        pauseMenuOverlayCanvas:setCanvasZOrder(100)
        _InitializeCanvasInputSystem(host, pauseMenuOverlayCanvas.luaProxyId)
        pauseMenuOverlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3,
                                                  "Opacity", UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        pauseMenuOverlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3,
                                                   "Opacity", UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local pauseMenuOverlay = UiOverlay:createOverlay(host, "PauseMenuOverlay", pauseMenuOverlayCanvas)

    -- ── Widgets ────────────────────────────────────────────────────────────────
    local scrim = UiRectangle:new(host, "PauseScrim")
    pauseMenuOverlay:addWidget(scrim)

    local card = UiRectangle:new(host, "PauseCard")
    pauseMenuOverlay:addWidget(card)

    local eyebrowLabel = UiLabel:new(host, FONT, "PauseEyebrow")
    pauseMenuOverlay:addWidget(eyebrowLabel)
    local titleLabel = UiLabel:new(host, FONT, "PauseTitle")
    pauseMenuOverlay:addWidget(titleLabel)
    -- Cyan-glow corner L-braces around the card (.pause-card__brace), 2 rects per corner.
    local braces = {}
    for i = 1, 8 do
        braces[i] = UiRectangle:new(host, "PauseBrace" .. tostring(i))
        pauseMenuOverlay:addWidget(braces[i])
    end
    local waveChip = UiRectangle:new(host, "PauseWaveChip")
    pauseMenuOverlay:addWidget(waveChip)
    local waveChipLabel = UiLabel:new(host, FONT, "PauseWaveChipLabel")
    pauseMenuOverlay:addWidget(waveChipLabel)
    local footerLabel = UiLabel:new(host, FONT, "PauseFooter")
    pauseMenuOverlay:addWidget(footerLabel)

    local items = buildItems(Combat)
    for _, item in ipairs(items) do
        item.bg = UiRectangle:new(host, "PauseItemBg_" .. item.id)
        pauseMenuOverlay:addWidget(item.bg)
        item.accentBar = UiRectangle:new(host, "PauseItemAccent_" .. item.id)
        pauseMenuOverlay:addWidget(item.accentBar)
        item.iconWidget = UiImage:new(host, "PauseItemIcon_" .. item.id)
        pauseMenuOverlay:addWidget(item.iconWidget)
        item.labelWidget = UiLabel:new(host, FONT, "PauseItemLabel_" .. item.id)
        pauseMenuOverlay:addWidget(item.labelWidget)
        item.subWidget = UiLabel:new(host, FONT, "PauseItemSub_" .. item.id)
        pauseMenuOverlay:addWidget(item.subWidget)
    end

    -- ── Confirmation dialog (for the destructive exits) ───────────────────────
    local confirmScrim = UiRectangle:new(host, "PauseConfirmScrim")
    pauseMenuOverlay:addWidget(confirmScrim)
    local confirmBox = UiRectangle:new(host, "PauseConfirmBox")
    pauseMenuOverlay:addWidget(confirmBox)
    local confirmTitleLabel = UiLabel:new(host, FONT, "PauseConfirmTitle")
    pauseMenuOverlay:addWidget(confirmTitleLabel)
    local confirmBodyLabel = UiLabel:new(host, FONT, "PauseConfirmBody")
    pauseMenuOverlay:addWidget(confirmBodyLabel)
    local confirmCancelBg = UiRectangle:new(host, "PauseConfirmCancelBg")
    pauseMenuOverlay:addWidget(confirmCancelBg)
    local confirmCancelLabel = UiLabel:new(host, FONT, "PauseConfirmCancelLabel")
    pauseMenuOverlay:addWidget(confirmCancelLabel)
    local confirmActionBg = UiRectangle:new(host, "PauseConfirmActionBg")
    pauseMenuOverlay:addWidget(confirmActionBg)
    local confirmActionLabel = UiLabel:new(host, FONT, "PauseConfirmActionLabel")
    pauseMenuOverlay:addWidget(confirmActionLabel)

    local confirmWidgets = {
        confirmScrim, confirmBox, confirmTitleLabel, confirmBodyLabel, confirmCancelBg, confirmCancelLabel,
        confirmActionBg, confirmActionLabel
    }

    local CONFIRM = {
        tomenu = {
            title = "ВЫЙТИ В ГЛАВНОЕ МЕНЮ?",
            body = "Прогресс текущей волны будет сохранён.",
            confirm = "ВЫЙТИ",
            action = function()
                EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                          "MainMenuLevel")
            end
        },
        quit = {
            title = "ЗАВЕРШИТЬ СЕАНС?",
            body = "Несохранённый прогресс будет потерян.",
            confirm = "ВЫЙТИ ИЗ ИГРЫ",
            action = function()
                EventsHelper:sendExitGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
            end
        }
    }

    local confirmAction = nil

    local function setConfirmVisible(isVisible)
        for _, w in ipairs(confirmWidgets) do w:setIsVisible(isVisible) end
        -- toggle input interception so the hidden dialog (full-screen scrim + buttons) does not block
        -- clicks on the pause menu underneath
        confirmScrim:setIfCanInterceptMouseInputEvent(isVisible)
        confirmCancelBg:setIfCanInterceptMouseInputEvent(isVisible)
        confirmActionBg:setIfCanInterceptMouseInputEvent(isVisible)
        -- The engine dispatches a click to every widget under the cursor (no top-most consumption), so a
        -- click on the dialog buttons would also fire the pause menu item sitting behind it. Disable the
        -- menu items' input while the dialog is open.
        for _, item in ipairs(items) do item.bg:setIsUiInputEnabled(not isVisible) end
    end

    local function hideConfirm()
        confirmAction = nil
        setConfirmVisible(false)
    end

    local function showConfirm(target)
        local data = CONFIRM[target]
        if not data then return end
        confirmTitleLabel:setText(data.title)
        confirmBodyLabel:setText(data.body)
        confirmActionLabel:setText(data.confirm)
        confirmAction = data.action
        setConfirmVisible(true)
    end

    -- ── Actions ──────────────────────────────────────────────────────────────
    local function runAction(id)
        if id == "resume" then
            EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
            UiOverlayManager:closeCurrentOverlay(host)
        elseif id == "settings" then
            UiOverlayManager:openOverlay(host, "GameSettingsOverlay")
        elseif id == "tomenu" or id == "quit" then
            showConfirm(id) -- destructive: ask first
        end
    end

    confirmCancelBg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        confirmCancelBg:setColorHexValue(
            newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Combat.chipHoverColor or Combat.chipColor)
    end)
    confirmCancelBg:subscribeOnMouseInputClickedCallback(function() hideConfirm() end)
    confirmActionBg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        confirmActionBg:setColorHexValue(
            newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Combat.dangerSoft or Combat.danger)
    end)
    confirmActionBg:subscribeOnMouseInputClickedCallback(function()
        if confirmAction then confirmAction() end
    end)

    for _, item in ipairs(items) do
        item.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            item.bg:setColorHexValue(
                newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Combat.chipHoverColor or Combat.chipColor)
        end)
        item.bg:subscribeOnMouseInputClickedCallback(function() 
             runAction(item.id) end)
    end

    -- ── Layout ───────────────────────────────────────────────────────────────
    pauseMenuOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        local A = UiItemBase.UiAnchorType
        local HALIGN = UiLabel.TextHorizontalAlignmentType
        local VALIGN = UiLabel.TextVerticalAlignmentType
        local canvasName = pauseMenuOverlayCanvas.widgetName
        -- a label's container height must clear the font's line height, otherwise the engine renders a
        -- literal "container.height < line.height" placeholder instead of the text
        local function lh(fontSize) return UiTextSizing.safeLineHeight(fontSize, FONT) end

        -- full-screen dimmer over the (paused) battlefield
        scrim:setParent(host, canvasName, canvasName)
        scrim:fill(canvasName)
        scrim:setColorHexValue(0x04060c)
        scrim:setOpacity(0.6)
        scrim:setZOrder(0)

        -- centered pause card (560px wide per .pause-card)
        local cardWidth = 560
        local cardHeight = 560
        card:setParent(host, canvasName, canvasName)
        card:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, canvasName, 0)
        card:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, canvasName, 0)
        card:setWidth(cardWidth)
        card:setHeight(cardHeight)
        card:setColorHexValue(Combat.panelColor)
        card:setOpacity(Combat.panelOpacity)
        card:setBorderRadius(Combat.panelBorderRadius)
        card:setZOrder(1)
        local cardName = card.widgetName

        -- corner L-braces (2 rects each: horizontal + vertical arm)
        local braceArm = 26
        local braceThick = 2
        local braceInset = 0
        local braceCorners = {
            {h = A.LEFT, v = A.TOP}, {h = A.RIGHT, v = A.TOP}, {h = A.LEFT, v = A.BOTTOM}, {h = A.RIGHT, v = A.BOTTOM}
        }
        for c = 1, 4 do
            local corner = braceCorners[c]
            local hArm = braces[(c - 1) * 2 + 1]
            local vArm = braces[(c - 1) * 2 + 2]
            for _, arm in ipairs({hArm, vArm}) do
                arm:setParent(host, canvasName, cardName)
                arm:setColorHexValue(Combat.cyanGlow)
                arm:setAnchor(corner.h, corner.h, cardName, braceInset)
                arm:setAnchor(corner.v, corner.v, cardName, braceInset)
                arm:setZOrder(3)
            end
            hArm:setWidth(braceArm)
            hArm:setHeight(braceThick)
            vArm:setWidth(braceThick)
            vArm:setHeight(braceArm)
        end

        eyebrowLabel:setParent(host, canvasName, cardName)
        eyebrowLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName, 0)
        eyebrowLabel:setAnchor(A.TOP, A.TOP, cardName, 28)
        eyebrowLabel:setWidth(cardWidth)
        eyebrowLabel:setHeight(lh(10))
        eyebrowLabel:setFontSize(10)
        eyebrowLabel:setTextColorHexValue(Combat.cyan)
        eyebrowLabel:setOpacity(0.6)
        eyebrowLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        eyebrowLabel:setTextVerticalAlignment(VALIGN.CENTER)
        eyebrowLabel:setText("— СИСТЕМА ПРИОСТАНОВЛЕНА —")
        eyebrowLabel:setZOrder(2)

        titleLabel:setParent(host, canvasName, cardName)
        titleLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName, 0)
        titleLabel:setAnchor(A.TOP, A.TOP, cardName, 48)
        titleLabel:setWidth(cardWidth)
        titleLabel:setHeight(lh(56))
        titleLabel:setFontSize(56)
        titleLabel:setTextColorHexValue(Combat.cyanGlow)
        titleLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
        titleLabel:setText("ПАУЗА")
        titleLabel:setZOrder(2)
        -- .pause-title gradient (135deg cyan-glow -> blue -> indigo); vertical 2-stop approximation.
        titleLabel:setTextGradientHexValues(UiLabel.TextGradientColorType.VERTICAL, Combat.cyanGlow, Combat.indigo)

        -- TODO: real wave number / phase once exposed to Lua
        waveChip:setParent(host, canvasName, cardName)
        waveChip:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName, 0)
        waveChip:setAnchor(A.TOP, A.TOP, cardName, 152)
        waveChip:setWidth(300)
        waveChip:setHeight(lh(11) + 8)
        waveChip:setColorHexValue(Combat.chipColor)
        waveChip:setOpacity(Combat.chipOpacity)
        waveChip:setBorderRadius(Combat.chipBorderRadius)
        waveChip:setZOrder(2)

        waveChipLabel:setParent(host, canvasName, waveChip.widgetName)
        waveChipLabel:fill(waveChip.widgetName)
        waveChipLabel:setFontSize(11)
        waveChipLabel:setTextColorHexValue(Combat.cyanGlow)
        waveChipLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        waveChipLabel:setTextVerticalAlignment(VALIGN.CENTER)
        waveChipLabel:setText("ВОЛНА 14 / 30 · ПОДГОТОВКА")
        waveChipLabel:setZOrder(3)

        -- menu items
        local itemWidth = cardWidth - 48
        local itemHeight = 64
        local itemSpacing = 12
        local firstItemTop = 196
        for i, item in ipairs(items) do
            item.bg:setParent(host, canvasName, cardName)
            item.bg:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName, 0)
            if i == 1 then
                item.bg:setAnchor(A.TOP, A.TOP, cardName, firstItemTop)
            else
                item.bg:setAnchor(A.TOP, A.BOTTOM, items[i - 1].bg.widgetName, itemSpacing)
            end
            item.bg:setWidth(itemWidth)
            item.bg:setHeight(itemHeight)
            item.bg:setColorHexValue(Combat.chipColor)
            item.bg:setBorderRadius(Combat.chipBorderRadius)
            item.bg:setZOrder(2)
            item.bg:enableMouseInputReceiverBase(host)
            item.bg:setIfCanInterceptMouseInputEvent(true)

            item.accentBar:setParent(host, canvasName, item.bg.widgetName)
            item.accentBar:setAnchor(A.LEFT, A.LEFT, item.bg.widgetName, 0)
            item.accentBar:setAnchor(A.TOP, A.TOP, item.bg.widgetName, 0)
            item.accentBar:setAnchor(A.BOTTOM, A.BOTTOM, item.bg.widgetName, 0)
            item.accentBar:setWidth(4)
            item.accentBar:setColorHexValue(item.accent)
            item.accentBar:setZOrder(3)

            -- icon (tinted with the item accent), left of the text
            item.iconWidget:setParent(host, canvasName, item.bg.widgetName)
            item.iconWidget:setAnchor(A.LEFT, A.LEFT, item.bg.widgetName, 18)
            item.iconWidget:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, item.bg.widgetName, 0)
            item.iconWidget:setWidth(26)
            item.iconWidget:setHeight(26)
            item.iconWidget:setTextureSource(item.icon)
            item.iconWidget:setUseImageCustomColor(true)
            item.iconWidget:setColorHexValue(item.accent)
            item.iconWidget:setZOrder(3)

            item.labelWidget:setParent(host, canvasName, item.bg.widgetName)
            item.labelWidget:setAnchor(A.LEFT, A.LEFT, item.bg.widgetName, 58)
            item.labelWidget:setAnchor(A.TOP, A.TOP, item.bg.widgetName, 8)
            item.labelWidget:setWidth(itemWidth - 76)
            item.labelWidget:setHeight(lh(17))
            item.labelWidget:setFontSize(17)
            item.labelWidget:setTextColorHexValue(item.accent)
            item.labelWidget:setTextHorizontalAlignment(HALIGN.LEFT)
            item.labelWidget:setTextVerticalAlignment(VALIGN.CENTER)
            item.labelWidget:setText(item.label)
            item.labelWidget:setZOrder(3)

            item.subWidget:setParent(host, canvasName, item.bg.widgetName)
            item.subWidget:setAnchor(A.LEFT, A.LEFT, item.bg.widgetName, 58)
            item.subWidget:setAnchor(A.BOTTOM, A.BOTTOM, item.bg.widgetName, 8)
            item.subWidget:setWidth(itemWidth - 76)
            item.subWidget:setHeight(lh(10))
            item.subWidget:setFontSize(10)
            item.subWidget:setTextColorHexValue(Combat.textDim)
            item.subWidget:setTextHorizontalAlignment(HALIGN.LEFT)
            item.subWidget:setTextVerticalAlignment(VALIGN.CENTER)
            item.subWidget:setText(item.sub)
            item.subWidget:setZOrder(3)
        end

        footerLabel:setParent(host, canvasName, cardName)
        footerLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName, 0)
        footerLabel:setAnchor(A.BOTTOM, A.BOTTOM, cardName, 16)
        footerLabel:setWidth(cardWidth)
        footerLabel:setHeight(lh(10))
        footerLabel:setFontSize(10)
        footerLabel:setTextColorHexValue(Combat.textVeryDim)
        footerLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        footerLabel:setTextVerticalAlignment(VALIGN.CENTER)
        footerLabel:setText("ESC — ПРОДОЛЖИТЬ")
        footerLabel:setZOrder(2)

        -- ── Confirmation dialog layout (hidden until a destructive item is chosen) ──
        confirmScrim:setParent(host, canvasName, canvasName)
        confirmScrim:fill(canvasName)
        confirmScrim:setColorHexValue(0x04060c)
        confirmScrim:setOpacity(0.7)
        confirmScrim:setZOrder(10)
        confirmScrim:enableMouseInputReceiverBase(host)
        confirmScrim:setIfCanInterceptMouseInputEvent(true)

        local confirmBoxW = 400
        local confirmBoxH = 200
        confirmBox:setParent(host, canvasName, canvasName)
        confirmBox:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, canvasName, 0)
        confirmBox:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, canvasName, 0)
        confirmBox:setWidth(confirmBoxW)
        confirmBox:setHeight(confirmBoxH)
        confirmBox:setColorHexValue(Combat.panelColor)
        confirmBox:setBorderRadius(Combat.panelBorderRadius)
        confirmBox:setZOrder(11)
        local confirmBoxName = confirmBox.widgetName

        confirmTitleLabel:setParent(host, canvasName, confirmBoxName)
        confirmTitleLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, confirmBoxName, 0)
        confirmTitleLabel:setAnchor(A.TOP, A.TOP, confirmBoxName, 26)
        confirmTitleLabel:setWidth(confirmBoxW - 32)
        confirmTitleLabel:setHeight(lh(18))
        confirmTitleLabel:setFontSize(18)
        confirmTitleLabel:setTextColorHexValue(Combat.textBright)
        confirmTitleLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        confirmTitleLabel:setTextVerticalAlignment(VALIGN.CENTER)
        confirmTitleLabel:setZOrder(12)

        confirmBodyLabel:setParent(host, canvasName, confirmBoxName)
        confirmBodyLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, confirmBoxName, 0)
        confirmBodyLabel:setAnchor(A.TOP, A.TOP, confirmBoxName, 70)
        confirmBodyLabel:setWidth(confirmBoxW - 32)
        confirmBodyLabel:setHeight(lh(12))
        confirmBodyLabel:setFontSize(12)
        confirmBodyLabel:setTextColorHexValue(Combat.textDim)
        confirmBodyLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        confirmBodyLabel:setTextVerticalAlignment(VALIGN.CENTER)
        confirmBodyLabel:setZOrder(12)

        local confirmBtnW = (confirmBoxW - 16 * 2 - 12) / 2
        confirmCancelBg:setParent(host, canvasName, confirmBoxName)
        confirmCancelBg:setAnchor(A.LEFT, A.LEFT, confirmBoxName, 16)
        confirmCancelBg:setAnchor(A.BOTTOM, A.BOTTOM, confirmBoxName, 16)
        confirmCancelBg:setWidth(confirmBtnW)
        confirmCancelBg:setHeight(42)
        confirmCancelBg:setColorHexValue(Combat.chipColor)
        confirmCancelBg:setBorderRadius(Combat.chipBorderRadius)
        confirmCancelBg:setZOrder(12)
        confirmCancelBg:enableMouseInputReceiverBase(host)
        confirmCancelBg:setIfCanInterceptMouseInputEvent(true)

        confirmCancelLabel:setParent(host, canvasName, confirmCancelBg.widgetName)
        confirmCancelLabel:fill(confirmCancelBg.widgetName)
        confirmCancelLabel:setFontSize(13)
        confirmCancelLabel:setTextColorHexValue(Combat.textBright)
        confirmCancelLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        confirmCancelLabel:setTextVerticalAlignment(VALIGN.CENTER)
        confirmCancelLabel:setText("ОТМЕНА")
        confirmCancelLabel:setZOrder(13)

        confirmActionBg:setParent(host, canvasName, confirmBoxName)
        confirmActionBg:setAnchor(A.RIGHT, A.RIGHT, confirmBoxName, 16)
        confirmActionBg:setAnchor(A.BOTTOM, A.BOTTOM, confirmBoxName, 16)
        confirmActionBg:setWidth(confirmBtnW)
        confirmActionBg:setHeight(42)
        confirmActionBg:setColorHexValue(Combat.danger)
        confirmActionBg:setBorderRadius(Combat.chipBorderRadius)
        confirmActionBg:setZOrder(12)
        confirmActionBg:enableMouseInputReceiverBase(host)
        confirmActionBg:setIfCanInterceptMouseInputEvent(true)

        confirmActionLabel:setParent(host, canvasName, confirmActionBg.widgetName)
        confirmActionLabel:fill(confirmActionBg.widgetName)
        confirmActionLabel:setFontSize(13)
        confirmActionLabel:setTextColorHexValue(Combat.textBright)
        confirmActionLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        confirmActionLabel:setTextVerticalAlignment(VALIGN.CENTER)
        confirmActionLabel:setZOrder(13)

        setConfirmVisible(false)
    end)

    pauseMenuOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    pauseMenuOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end
    pauseMenuOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return pauseMenuOverlay
end

return PauseOverlay
