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
local UiCanvas = require("core/uiCanvas")
local UiOverlay = require("core/uiOverlay")
local UiRectangle = require("core/uiRectangle")
local UiLabel = require("core/uiLabel")
local UiToggleButton = require("core/uiToggleButton")

SettingsOverlay = {
    buttonColor = 0x403649,
    hoveredButtonColor = 0x201b24,
    buttonRadius = 6
}

function SettingsOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseSettingsOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    pauseSettingsOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, pauseSettingsOverlayCanvas.luaProxyId)
        pauseSettingsOverlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        pauseSettingsOverlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local pauseSettingsOverlay = UiOverlay:createOverlay(host, "PauseSettingsOverlay", pauseSettingsOverlayCanvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 7.0;

    local rowButtonsCount = 2.0;
    local backgroundRectWidth = windowWidth - (menuHorizontalMargin * 2.0)
    local buttonHorizontalMargin = backgroundRectWidth / 10.0;
    local buttonWidth = (backgroundRectWidth - (buttonHorizontalMargin * (rowButtonsCount + 1.0))) / rowButtonsCount;

    local backgroundRect = UiRectangle:new(host)
    pauseSettingsOverlay:addWidget(backgroundRect)

    local soundToggleButton = UiToggleButton:new(host, false)
    pauseSettingsOverlay:addWidget(soundToggleButton)
    soundToggleButton:setOnIsStateChangedCallback(function(newState)
    end)

    local soundLabel = UiLabel:new(host, "nimbus_mono")
    pauseSettingsOverlay:addWidget(soundLabel)

    local applyButton = UiRectangle:new(host)
    pauseSettingsOverlay:addWidget(applyButton)
    applyButton:setOnMouseInputClickedCallback(function()
        UiOverlayManager:openOverlay(host, "PauseMenuOverlay")
    end)
    applyButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            applyButton:setColorHexValue(SettingsOverlay.hoveredButtonColor)
        else
            applyButton:setColorHexValue(SettingsOverlay.buttonColor)
        end
    end)

    local applyButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseSettingsOverlay:addWidget(applyButtonLabel)

    local cancelButton = UiRectangle:new(host)
    pauseSettingsOverlay:addWidget(cancelButton)
    cancelButton:setOnMouseInputClickedCallback(function()
        UiOverlayManager:openOverlay(host, "PauseMenuOverlay")
    end)
    cancelButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            cancelButton:setColorHexValue(SettingsOverlay.hoveredButtonColor)
        else
            cancelButton:setColorHexValue(SettingsOverlay.buttonColor)
        end
    end)

    local cancelButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseSettingsOverlay:addWidget(cancelButtonLabel)

    pauseSettingsOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("pauseSettingsOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, pauseSettingsOverlayCanvas.widgetName, pauseSettingsOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            pauseSettingsOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            pauseSettingsOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            pauseSettingsOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            pauseSettingsOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(0x6C5B7B)
        backgroundRect:setZOrder(1)
        backgroundRect:setBorderRadius(SettingsOverlay.buttonRadius)

        soundToggleButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, buttonHorizontalMargin)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            50)
        soundToggleButton:setWidth(buttonWidth / 4.0)
        soundToggleButton:setHeight(buttonWidth / 4.0)
        soundToggleButton:setZOrder(2)
        soundToggleButton:setToggleOnColorHexValue(0xFFB732)
        soundToggleButton:setToggleOffColorHexValue(SettingsOverlay.buttonColor)
        soundToggleButton:enableToggleButtonMouseInputReceiver(host)

        soundLabel:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.LEFT, soundToggleButton.widgetName,
            buttonHorizontalMargin)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName, 50)
        soundLabel:setHeight(buttonWidth / 4.0)
        soundLabel:setText("Enable sound effects")
        soundLabel:setTextColorHexValue(0xFFFFFF)
        soundLabel:setFontSize(11.0)
        soundLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        soundLabel:setZOrder(2)

        applyButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        applyButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin)
        applyButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName,
            50)
        applyButton:setWidth(buttonWidth)
        applyButton:setHeight(100)
        applyButton:setColorHexValue(SettingsOverlay.buttonColor)
        applyButton:setZOrder(2)
        applyButton:enableMouseInputReceiverBase(host)
        applyButton:setBorderRadius(SettingsOverlay.buttonRadius)

        applyButtonLabel:setParent(host, pauseSettingsOverlayCanvas.widgetName, applyButton.widgetName)
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, applyButton.widgetName, 0)
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, applyButton.widgetName,
            0)
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, applyButton.widgetName, 0)
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, applyButton
            .widgetName, 0)
        applyButtonLabel:setText("Apply")
        applyButtonLabel:setTextColorHexValue(0xFFFFFF)
        applyButtonLabel:setFontSize(20.0)
        applyButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        applyButtonLabel:setZOrder(3)

        cancelButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        cancelButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, applyButton.widgetName,
            buttonHorizontalMargin)
        cancelButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName,
            50)
        cancelButton:setWidth(buttonWidth)
        cancelButton:setHeight(100)
        cancelButton:setColorHexValue(SettingsOverlay.buttonColor)
        cancelButton:setZOrder(2)
        cancelButton:enableMouseInputReceiverBase(host)
        cancelButton:setBorderRadius(SettingsOverlay.buttonRadius)

        cancelButtonLabel:setParent(host, pauseSettingsOverlayCanvas.widgetName, cancelButton.widgetName)
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cancelButton.widgetName,
            0)
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cancelButton
            .widgetName, 0)
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cancelButton.widgetName, 0)
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            cancelButton.widgetName, 0)
        cancelButtonLabel:setText("Cancel")
        cancelButtonLabel:setTextColorHexValue(0xFFFFFF)
        cancelButtonLabel:setFontSize(20.0)
        cancelButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        cancelButtonLabel:setZOrder(3)
    end)

    pauseSettingsOverlay.onGameEventTriggered = function(eventName, jsonArgs) end

    return pauseSettingsOverlay
end

return SettingsOverlay
