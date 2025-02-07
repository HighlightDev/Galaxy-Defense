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
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local UiToggleButton = require("Ui/Core/uiToggleButton")
local UiProgressBar = require("Ui/Core/uiProgressBar")
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")

SettingsOverlay = {
    buttonRadius = 6
}

function SettingsOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseSettingsOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "PauseSettingsCanvas")
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

    local applyButton = LabelButton:new(host, pauseSettingsOverlay, "nimbus_mono", "ApplyButton")
    pauseSettingsOverlay:addCompoundWidget(applyButton)
    applyButton:subscribeOnMouseInputClickedCallback(function()
        UiOverlayManager:closeCurrentOverlay(host)
    end)
    applyButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            applyButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            applyButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local testProgressBar = UiProgressBar:new(host, "TEST_PROGRESS_BAR")
    pauseSettingsOverlay:addWidget(testProgressBar)

    local cancelButton = LabelButton:new(host, pauseSettingsOverlay, "nimbus_mono", "CancelButton")
    pauseSettingsOverlay:addCompoundWidget(cancelButton)
    cancelButton:subscribeOnMouseInputClickedCallback(function()
        UiOverlayManager:closeCurrentOverlay(host)
    end)
    cancelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            cancelButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            cancelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

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
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
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
        soundToggleButton:setToggleOffColorHexValue(Styles.Colors.buttonColor)
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

        testProgressBar:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        testProgressBar:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            100.0)
        testProgressBar:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect
            .widgetName, 100.0)
        testProgressBar:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, soundLabel.widgetName,
            30.0)
        testProgressBar:setHeight(50.0)
        testProgressBar:setZOrder(2)
        testProgressBar:setFillPercentValue(0.25)
        testProgressBar:setEmptyColorHexValue(0xAAEEFF)
        testProgressBar:setFilledColorHexValue(0xFFEEAA)

        applyButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        applyButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin)
        applyButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName,
            50)
        applyButton:setWidth(buttonWidth)
        applyButton:setHeight(100)
        applyButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        applyButton:setZOrder(2)
        applyButton:setButtonBorderRadius(SettingsOverlay.buttonRadius)
        applyButton:setLabelText("Apply")
        applyButton:setLabelTextColorHexValue(0xFFFFFF)
        applyButton:setLabelFontSize(20.0)
        applyButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)

        cancelButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        cancelButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, applyButton.widgetName,
            buttonHorizontalMargin)
        cancelButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName,
            50)
        cancelButton:setWidth(buttonWidth)
        cancelButton:setHeight(100)
        cancelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        cancelButton:setZOrder(2)
        cancelButton:setButtonBorderRadius(SettingsOverlay.buttonRadius)
        cancelButton:setLabelText("Cancel")
        cancelButton:setLabelTextColorHexValue(0xFFFFFF)
        cancelButton:setLabelFontSize(20.0)
        cancelButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
    end)

    pauseSettingsOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    pauseSettingsOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    return pauseSettingsOverlay
end

return SettingsOverlay
