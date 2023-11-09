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
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]

local UiOverlayManager = require("core/uiOverlayManager")
local EngineEventsHolder = require("core/engineEventsHolder")
local UiCanvas = require("core/uiCanvas")
local UiOverlay = require("core/uiOverlay")
local UiRectangle = require("core/uiRectangle")
local UiLabel = require("core/uiLabel")

PauseOverlay = {
    buttonColor = 0x403649,
    hoveredButtonColor = 0x201b24,
    buttonRadius = 6
}

function PauseOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseMenuOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
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

    local continueButton = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(continueButton)
    continueButton:setOnMouseInputClickedCallback(function()
        EngineEventsHolder:sendPauseGameThreadEvent(host,
            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
        UiOverlayManager:closeCurrentOverlay(host)
        UiOverlayManager:openBackgroundOverlay(host, "PlayerHUDOverlay")
    end)
    continueButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            continueButton:setColorHexValue(PauseOverlay.hoveredButtonColor)
        else
            continueButton:setColorHexValue(PauseOverlay.buttonColor)
        end
    end)

    local continueButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseMenuOverlay:addWidget(continueButtonLabel)

    local settingsButton = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(settingsButton)
    settingsButton:setOnMouseInputClickedCallback(function()
        UiOverlayManager:openOverlay(host, "PauseSettingsOverlay")
    end)
    settingsButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            settingsButton:setColorHexValue(PauseOverlay.hoveredButtonColor)
        else
            settingsButton:setColorHexValue(PauseOverlay.buttonColor)
        end
    end)

    local settingsButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseMenuOverlay:addWidget(settingsButtonLabel)

    local exitToMainMenuButton = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(exitToMainMenuButton)
    exitToMainMenuButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitToMainMenuButton:setColorHexValue(PauseOverlay.hoveredButtonColor)
        else
            exitToMainMenuButton:setColorHexValue(PauseOverlay.buttonColor)
        end
    end)
    exitToMainMenuButton:setOnMouseInputClickedCallback(function()
        EngineEventsHolder:sendLoadLevelGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, "MainMenuLevel")
    end)

    local exitToMainMenuButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseMenuOverlay:addWidget(exitToMainMenuButtonLabel)

    local exitGameButton = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(exitGameButton)
    exitGameButton:setOnMouseInputClickedCallback(function()
        EngineEventsHolder:sendExitGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    exitGameButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitGameButton:setColorHexValue(PauseOverlay.hoveredButtonColor)
        else
            exitGameButton:setColorHexValue(PauseOverlay.buttonColor)
        end
    end)

    local exitGameMenuButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseMenuOverlay:addWidget(exitGameMenuButtonLabel)

    pauseMenuOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("pauseMenuOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, pauseMenuOverlayCanvas.widgetName, pauseMenuOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            pauseMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            pauseMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            pauseMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            pauseMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(0x6C5B7B)
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
        continueButton:setColorHexValue(PauseOverlay.buttonColor)
        continueButton:setZOrder(2)
        continueButton:enableMouseInputReceiverBase(host)
        continueButton:setBorderRadius(PauseOverlay.buttonRadius)

        continueButtonLabel:setParent(host, pauseMenuOverlayCanvas.widgetName, continueButton.widgetName)
        continueButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            continueButton.widgetName)
        continueButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            continueButton.widgetName)
        continueButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, continueButton
            .widgetName)
        continueButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            continueButton.widgetName)
        continueButtonLabel:setText("Continue")
        continueButtonLabel:setTextColorHexValue(0xFFFFFF)
        continueButtonLabel:setFontSize(20.0)
        continueButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        continueButtonLabel:setZOrder(3)

        settingsButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, continueButton.widgetName,
            buttonVerticalMarginHeight)
        settingsButton:setHeight(buttonHeight)
        settingsButton:setColorHexValue(PauseOverlay.buttonColor)
        settingsButton:setZOrder(2)
        settingsButton:enableMouseInputReceiverBase(host)
        settingsButton:setBorderRadius(PauseOverlay.buttonRadius)

        settingsButtonLabel:setParent(host, pauseMenuOverlayCanvas.widgetName, settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, settingsButton
            .widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName)
        settingsButtonLabel:setText("Settings")
        settingsButtonLabel:setTextColorHexValue(0xFFFFFF)
        settingsButtonLabel:setFontSize(20.0)
        settingsButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        settingsButtonLabel:setZOrder(3)

        exitToMainMenuButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            backgroundRect.widgetName, 20)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        exitToMainMenuButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName, buttonVerticalMarginHeight)
        exitToMainMenuButton:setHeight(buttonHeight)
        exitToMainMenuButton:setColorHexValue(PauseOverlay.buttonColor)
        exitToMainMenuButton:setZOrder(2)
        exitToMainMenuButton:enableMouseInputReceiverBase(host)
        exitToMainMenuButton:setBorderRadius(PauseOverlay.buttonRadius)

        exitToMainMenuButtonLabel:setParent(host, pauseMenuOverlayCanvas.widgetName, exitToMainMenuButton.widgetName)
        exitToMainMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            exitToMainMenuButton.widgetName)
        exitToMainMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            exitToMainMenuButton.widgetName)
        exitToMainMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            exitToMainMenuButton.widgetName)
        exitToMainMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            exitToMainMenuButton.widgetName)
        exitToMainMenuButtonLabel:setText("Exit to main menu")
        exitToMainMenuButtonLabel:setTextColorHexValue(0xFFFFFF)
        exitToMainMenuButtonLabel:setFontSize(20.0)
        exitToMainMenuButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitToMainMenuButtonLabel:setZOrder(3)

        exitGameButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            exitToMainMenuButton.widgetName, buttonVerticalMarginHeight)
        exitGameButton:setHeight(buttonHeight)
        exitGameButton:setColorHexValue(PauseOverlay.buttonColor)
        exitGameButton:setZOrder(2)
        exitGameButton:enableMouseInputReceiverBase(host)
        exitGameButton:setBorderRadius(PauseOverlay.buttonRadius)

        exitGameMenuButtonLabel:setParent(host, pauseMenuOverlayCanvas.widgetName, exitGameButton.widgetName)
        exitGameMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            exitGameButton.widgetName)
        exitGameMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            exitGameButton.widgetName)
        exitGameMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            exitGameButton.widgetName)
        exitGameMenuButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            exitGameButton.widgetName)
        exitGameMenuButtonLabel:setText("Exit game")
        exitGameMenuButtonLabel:setTextColorHexValue(0xFFFFFF)
        exitGameMenuButtonLabel:setFontSize(20.0)
        exitGameMenuButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitGameMenuButtonLabel:setZOrder(3)
    end)

    pauseMenuOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    pauseMenuOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    return pauseMenuOverlay;
end

return PauseOverlay
