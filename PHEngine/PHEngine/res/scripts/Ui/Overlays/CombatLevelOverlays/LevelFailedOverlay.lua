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
        package.path = package.path .. "" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiItem = require("Ui/Core/uiItem")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")
local UiRowLayout = require("Ui/Core/uiRowLayout")
local WeaponTile = require("Ui/Widgets/WeaponTile")
local ImageAndLabelTile = require("Ui/Widgets/ImageAndLabelTile")
local LabelButton = require("Ui/Widgets/LabelButton")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")

LevelFailedOverlay = {buttonRadius = 6}

function LevelFailedOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local levelFailedOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    levelFailedOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, levelFailedOverlayCanvas.luaProxyId)
    end)
    local levelFailedOverlay = UiOverlay:createOverlay(host, "LevelFailedOverlay", levelFailedOverlayCanvas)

    local backgroundRect = UiRectangle:new(host)
    levelFailedOverlay:addWidget(backgroundRect)

    local menuHorizontalMargin = windowWidth / 3.0;
    local menuVerticalMargin = windowHeight / 4.0;

    local backgroundRectWidth = windowWidth - (menuHorizontalMargin * 2.0)
    local buttonHorizontalMargin = 30;
    local buttonVerticalMargin = 30;
    local buttonWidth = backgroundRectWidth * 0.75

    local backgroundRect = UiRectangle:new(host)
    levelFailedOverlay:addWidget(backgroundRect)

    local descriptionLabel = UiLabel:new(host, "Lora-VariableFont_wght")
    levelFailedOverlay:addWidget(descriptionLabel)

    local restartButton = LabelButton:new(host, levelFailedOverlay, "Lora-VariableFont_wght", "RestartButton")
    levelFailedOverlay:addCompoundWidget(restartButton)
    restartButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendRestartLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    restartButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            restartButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            restartButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local exitToMainMenuButton = LabelButton:new(host, levelFailedOverlay, "Lora-VariableFont_wght",
                                                 "ExitToMainMenuButton")
    levelFailedOverlay:addCompoundWidget(exitToMainMenuButton)
    exitToMainMenuButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "MainMenuLevel")
    end)
    exitToMainMenuButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    levelFailedOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("levelFailedOverlay:OnAllWidgetLuaProxiesReady: name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, levelFailedOverlayCanvas.widgetName, levelFailedOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                 levelFailedOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                 levelFailedOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                 levelFailedOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                 levelFailedOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
        backgroundRect:setZOrder(1)
        backgroundRect:setBorderRadius(SettingsOverlay.buttonRadius)

        descriptionLabel:setParent(host, levelFailedOverlayCanvas.widgetName, backgroundRect.widgetName)
        descriptionLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                   backgroundRect.widgetName, buttonHorizontalMargin)
        descriptionLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                   backgroundRect.widgetName, buttonHorizontalMargin)
        descriptionLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
                                   50)
        descriptionLabel:setHeight(buttonWidth / 4.0)
        descriptionLabel:setText("Level Failed")
        descriptionLabel:setTextColorHexValue(0xFFFFFF)
        descriptionLabel:setFontSize(40.0)
        descriptionLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        descriptionLabel:setZOrder(2)

        restartButton:setParent(host, levelFailedOverlayCanvas.widgetName, backgroundRect.widgetName)
        restartButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                descriptionLabel.widgetName, buttonVerticalMargin * 2)
        restartButton:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                backgroundRect.widgetName)
        restartButton:setWidth(buttonWidth)
        restartButton:setHeight(100)
        restartButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        restartButton:setZOrder(2)
        restartButton:setButtonBorderRadius(LevelFailedOverlay.buttonRadius)
        restartButton:setLabelText("Restart Level")
        restartButton:setLabelTextColorHexValue(0xFFFFFF)
        restartButton:setLabelFontSize(26)
        restartButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        restartButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        restartButton:setPressStateButtonColorHexValues(0x000000)

        exitToMainMenuButton:setParent(host, levelFailedOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                       restartButton.widgetName, buttonVerticalMargin)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                       UiItemBase.UiAnchorType.HORIZONTAL_CENTER, backgroundRect.widgetName)
        exitToMainMenuButton:setWidth(buttonWidth)
        exitToMainMenuButton:setHeight(100)
        exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        exitToMainMenuButton:setZOrder(2)
        exitToMainMenuButton:setButtonBorderRadius(LevelFailedOverlay.buttonRadius)
        exitToMainMenuButton:setLabelText("Exit to main menu")
        exitToMainMenuButton:setLabelTextColorHexValue(0xFFFFFF)
        exitToMainMenuButton:setLabelFontSize(26)
        exitToMainMenuButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitToMainMenuButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        exitToMainMenuButton:setPressStateButtonColorHexValues(0x000000)
    end)

    levelFailedOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    levelFailedOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end
    levelFailedOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs) end

    return levelFailedOverlay
end

return LevelFailedOverlay
