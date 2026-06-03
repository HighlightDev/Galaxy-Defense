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
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local LabelButton = require("Ui/Widgets/LabelButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")

-- Defeat screen styled after UiMockup/menu-end-centered.css (the defeat variant): dark Combat
-- surface, red/danger accents, Orbitron-style eyebrow + title, bracketed action buttons. The full
-- mockup also carries a stats/medals/autopsy grid, but that needs run data the engine doesn't expose
-- yet, so only the title block + actions are reproduced here.
LevelFailedOverlay = {buttonRadius = 6}

local FONT = "JetBrainsMono-VariableFont_wght"

function LevelFailedOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)
    local C = Styles.Combat
    local A = UiItemBase.UiAnchorType
    local HALIGN = UiLabel.TextHorizontalAlignmentType
    local VALIGN = UiLabel.TextVerticalAlignmentType
    local function lh(fontSize) return UiTextSizing.safeLineHeight(fontSize, FONT) end

    local levelFailedOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    levelFailedOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, levelFailedOverlayCanvas.luaProxyId)
    end)
    local levelFailedOverlay = UiOverlay:createOverlay(host, "LevelFailedOverlay", levelFailedOverlayCanvas)

    local scrim = UiRectangle:new(host, "LevelFailedScrim")
    levelFailedOverlay:addWidget(scrim)

    local card = UiRectangle:new(host, "LevelFailedCard")
    levelFailedOverlay:addWidget(card)

    -- Danger-tinted corner L-braces (2 rects per corner).
    local braces = {}
    for i = 1, 8 do
        braces[i] = UiRectangle:new(host, "LevelFailedBrace" .. tostring(i))
        levelFailedOverlay:addWidget(braces[i])
    end

    local eyebrowLabel = UiLabel:new(host, FONT, "LevelFailedEyebrow")
    levelFailedOverlay:addWidget(eyebrowLabel)
    local titleLabel = UiLabel:new(host, FONT, "LevelFailedTitle")
    levelFailedOverlay:addWidget(titleLabel)
    local subLabel = UiLabel:new(host, FONT, "LevelFailedSub")
    levelFailedOverlay:addWidget(subLabel)

    local restartButton = LabelButton:new(host, levelFailedOverlay, FONT, "RestartButton")
    levelFailedOverlay:addCompoundWidget(restartButton)
    restartButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendRestartLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    restartButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            restartButton:setButtonColorHexValue(C.chipHoverColor)
        else
            restartButton:setButtonColorHexValue(C.chipColor)
        end
    end)

    local exitToMainMenuButton = LabelButton:new(host, levelFailedOverlay, FONT, "ExitToMainMenuButton")
    levelFailedOverlay:addCompoundWidget(exitToMainMenuButton)
    exitToMainMenuButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "MainMenuLevel")
    end)
    exitToMainMenuButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitToMainMenuButton:setButtonColorHexValue(C.chipHoverColor)
        else
            exitToMainMenuButton:setButtonColorHexValue(C.chipColor)
        end
    end)

    levelFailedOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("levelFailedOverlay:OnAllWidgetLuaProxiesReady: name: " .. tostring(sender.overlayName))
        local cn = levelFailedOverlayCanvas.widgetName

        -- Full-screen dimmer.
        scrim:setParent(host, cn, cn)
        scrim:fill(cn)
        scrim:setColorHexValue(0x04060c)
        scrim:setOpacity(0.72)
        scrim:setZOrder(0)

        -- Centered defeat card.
        local cardWidth = math.floor(windowWidth * 0.34)
        local cardHeight = math.floor(windowHeight * 0.46)
        card:setParent(host, cn, cn)
        card:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cn)
        card:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, cn)
        card:setWidth(cardWidth)
        card:setHeight(cardHeight)
        card:setColorHexValue(C.panelColor)
        card:setOpacity(C.panelOpacity)
        card:setBorderRadius(C.panelBorderRadius)
        card:setZOrder(1)
        local cardName = card.widgetName

        -- Corner braces.
        local braceArm = 26
        local braceThick = 2
        local braceCorners = {
            {h = A.LEFT, v = A.TOP}, {h = A.RIGHT, v = A.TOP}, {h = A.LEFT, v = A.BOTTOM}, {h = A.RIGHT, v = A.BOTTOM}
        }
        for c = 1, 4 do
            local corner = braceCorners[c]
            local hArm = braces[(c - 1) * 2 + 1]
            local vArm = braces[(c - 1) * 2 + 2]
            for _, arm in ipairs({hArm, vArm}) do
                arm:setParent(host, cn, cardName)
                arm:setColorHexValue(C.dangerSoft)
                arm:setAnchor(corner.h, corner.h, cardName, 0)
                arm:setAnchor(corner.v, corner.v, cardName, 0)
                arm:setZOrder(3)
            end
            hArm:setWidth(braceArm)
            hArm:setHeight(braceThick)
            vArm:setWidth(braceThick)
            vArm:setHeight(braceArm)
        end

        eyebrowLabel:setParent(host, cn, cardName)
        eyebrowLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName)
        eyebrowLabel:setAnchor(A.TOP, A.TOP, cardName, 40)
        eyebrowLabel:setWidth(cardWidth)
        eyebrowLabel:setHeight(lh(11))
        eyebrowLabel:setFontSize(11)
        eyebrowLabel:setTextColorHexValue(C.dangerSoft)
        eyebrowLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        eyebrowLabel:setTextVerticalAlignment(VALIGN.CENTER)
        eyebrowLabel:setText("— ОБОРОНА ПРОВАЛЕНА —")
        eyebrowLabel:setZOrder(2)

        titleLabel:setParent(host, cn, cardName)
        titleLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName)
        titleLabel:setAnchor(A.TOP, A.TOP, cardName, 64)
        titleLabel:setWidth(cardWidth)
        titleLabel:setHeight(lh(58))
        titleLabel:setFontSize(58)
        titleLabel:setTextColorHexValue(C.dangerSoft)
        titleLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
        titleLabel:setText("ПОРАЖЕНИЕ")
        titleLabel:setZOrder(2)
        -- .end-title__h--defeat gradient (135deg soft-red -> danger-red -> lilac); vertical 2-stop red ramp.
        titleLabel:setTextGradientHexValues(UiLabel.TextGradientColorType.VERTICAL, C.dangerSoft, C.danger)

        subLabel:setParent(host, cn, cardName)
        subLabel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName)
        subLabel:setAnchor(A.TOP, A.TOP, cardName, 64 + lh(58) + 8)
        subLabel:setWidth(cardWidth)
        subLabel:setHeight(lh(13))
        subLabel:setFontSize(13)
        subLabel:setTextColorHexValue(C.textDim)
        subLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        subLabel:setTextVerticalAlignment(VALIGN.CENTER)
        subLabel:setText("Станция потеряна · рубеж прорван")
        subLabel:setZOrder(2)

        -- Action buttons.
        local buttonWidth = math.floor(cardWidth * 0.68)
        local buttonHeight = 60
        local buttonGap = 16

        restartButton:setParent(host, cn, cardName)
        restartButton:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName)
        restartButton:setAnchor(A.BOTTOM, A.BOTTOM, cardName, 40 + buttonHeight + buttonGap)
        restartButton:setWidth(buttonWidth)
        restartButton:setHeight(buttonHeight)
        restartButton:setButtonColorHexValue(C.chipColor)
        restartButton:setZOrder(2)
        restartButton:setButtonBorderRadius(LevelFailedOverlay.buttonRadius)
        restartButton:setLabelText("ПОВТОРИТЬ ВОЛНУ")
        restartButton:setLabelTextColorHexValue(C.cyanGlow)
        restartButton:setLabelFontSize(18)
        restartButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
        restartButton:setLabelTextVerticalAlignment(VALIGN.CENTER)
        restartButton:setPressStateButtonColorHexValues(0x000000)

        exitToMainMenuButton:setParent(host, cn, cardName)
        exitToMainMenuButton:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, cardName)
        exitToMainMenuButton:setAnchor(A.BOTTOM, A.BOTTOM, cardName, 40)
        exitToMainMenuButton:setWidth(buttonWidth)
        exitToMainMenuButton:setHeight(buttonHeight)
        exitToMainMenuButton:setButtonColorHexValue(C.chipColor)
        exitToMainMenuButton:setZOrder(2)
        exitToMainMenuButton:setButtonBorderRadius(LevelFailedOverlay.buttonRadius)
        exitToMainMenuButton:setLabelText("В ГЛАВНОЕ МЕНЮ")
        exitToMainMenuButton:setLabelTextColorHexValue(C.textBright)
        exitToMainMenuButton:setLabelFontSize(18)
        exitToMainMenuButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
        exitToMainMenuButton:setLabelTextVerticalAlignment(VALIGN.CENTER)
        exitToMainMenuButton:setPressStateButtonColorHexValues(0x000000)
    end)

    levelFailedOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    levelFailedOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end
    levelFailedOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return levelFailedOverlay
end

return LevelFailedOverlay
