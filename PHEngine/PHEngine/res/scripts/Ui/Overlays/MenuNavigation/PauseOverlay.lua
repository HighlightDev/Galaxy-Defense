--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
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
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")

PauseOverlay = {
    buttonRadius = 6
}

function PauseOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseMenuOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "PauseMenuCanvas")
    pauseMenuOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        pauseMenuOverlayCanvas:setCanvasZOrder(100)
        _InitializeCanvasInputSystem(host, pauseMenuOverlayCanvas.luaProxyId)
        pauseMenuOverlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        pauseMenuOverlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local pauseMenuOverlay = UiOverlay:createOverlay(host, "PauseMenuOverlay", pauseMenuOverlayCanvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 15.0;

    local backgroundRect = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(backgroundRect)

    local pauseMenuHeight = (windowHeight - (menuVerticalMargin * 2.0))
    local buttonsCount = 4;
    local buttonsMarginCount = buttonsCount + 1;
    local buttonHeight = (pauseMenuHeight / buttonsCount)
    local buttonVerticalMarginHeight = buttonHeight / 4.0;
    local totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
    buttonHeight = (pauseMenuHeight - totalButtonMarginHeight) / buttonsCount;

    local continueButton = LabelButton:new(host, pauseMenuOverlay, "Lora-VariableFont_wght", "ContinueButton")
    pauseMenuOverlay:addCompoundWidget(continueButton)
    continueButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendPauseGameThreadEvent(host,
            EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
        UiOverlayManager:closeCurrentOverlay(host)
    end)
    continueButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            continueButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            continueButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local settingsButton = LabelButton:new(host, pauseMenuOverlay, "Lora-VariableFont_wght", "SettingsButton")
    pauseMenuOverlay:addCompoundWidget(settingsButton)
    settingsButton:subscribeOnMouseInputClickedCallback(function()
        UiOverlayManager:openOverlay(host, "PauseSettingsOverlay")
    end)
    settingsButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            settingsButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            settingsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local exitToMainMenuButton = LabelButton:new(host, pauseMenuOverlay, "Lora-VariableFont_wght", "ExitToMainMenuButton")
    pauseMenuOverlay:addCompoundWidget(exitToMainMenuButton)
    exitToMainMenuButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)
    exitToMainMenuButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "MainMenuLevel")
    end)

    local exitGameButton = LabelButton:new(host, pauseMenuOverlay, "Lora-VariableFont_wght", "ExitGameButton")
    pauseMenuOverlay:addCompoundWidget(exitGameButton)
    exitGameButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendExitGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    exitGameButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitGameButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            exitGameButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    pauseMenuOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("pauseMenuOverlay:OnAllWidgetLuaProxiesReady: name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, pauseMenuOverlayCanvas.widgetName, pauseMenuOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            pauseMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            pauseMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            pauseMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            pauseMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
        backgroundRect:setZOrder(1)
        backgroundRect:setBorderRadius(PauseOverlay.buttonRadius)

        continueButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        continueButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        continueButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        continueButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            buttonVerticalMarginHeight)
        continueButton:setHeight(buttonHeight)
        continueButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        continueButton:setZOrder(2)
        continueButton:setButtonBorderRadius(PauseOverlay.buttonRadius)
        continueButton:setLabelText("Continue")
        continueButton:setLabelTextColorHexValue(0xFFFFFF)
        continueButton:setLabelFontSize(56)
        continueButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        continueButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        continueButton:setPressStateButtonColorHexValues(0x000000)

        settingsButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, continueButton.widgetName,
            buttonVerticalMarginHeight)
        settingsButton:setHeight(buttonHeight)
        settingsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        settingsButton:setZOrder(2)
        settingsButton:setButtonBorderRadius(PauseOverlay.buttonRadius)
        settingsButton:setLabelText("Settings")
        settingsButton:setLabelTextColorHexValue(0xFFFFFF)
        settingsButton:setLabelFontSize(56)
        settingsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        settingsButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        settingsButton:setPressStateButtonColorHexValues(0x000000)

        exitToMainMenuButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            backgroundRect.widgetName, 20)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName, buttonVerticalMarginHeight)
        exitToMainMenuButton:setHeight(buttonHeight)
        exitToMainMenuButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        exitToMainMenuButton:setZOrder(2)
        exitToMainMenuButton:setButtonBorderRadius(PauseOverlay.buttonRadius)
        exitToMainMenuButton:setLabelText("Exit to main menu")
        exitToMainMenuButton:setLabelTextColorHexValue(0xFFFFFF)
        exitToMainMenuButton:setLabelFontSize(56)
        exitToMainMenuButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitToMainMenuButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        exitToMainMenuButton:setPressStateButtonColorHexValues(0x000000)

        exitGameButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            exitToMainMenuButton.widgetName, buttonVerticalMarginHeight)
        exitGameButton:setHeight(buttonHeight)
        exitGameButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        exitGameButton:setZOrder(2)
        exitGameButton:setButtonBorderRadius(PauseOverlay.buttonRadius)
        exitGameButton:setLabelText("Exit game")
        exitGameButton:setLabelTextColorHexValue(0xFFFFFF)
        exitGameButton:setLabelFontSize(56)
        exitGameButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitGameButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        exitGameButton:setPressStateButtonColorHexValues(0x000000)
    end)

    pauseMenuOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    pauseMenuOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    return pauseMenuOverlay;
end

return PauseOverlay
