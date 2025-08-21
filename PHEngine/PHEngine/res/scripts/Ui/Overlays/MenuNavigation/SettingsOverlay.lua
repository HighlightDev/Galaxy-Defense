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
local UiToggleButton = require("Ui/Core/uiToggleButton")
local UiProgressBar = require("Ui/Core/uiProgressBar")
local UiSlider = require("Ui/Core/uiSlider")
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")
local json = require("Ui/Core/3rdparty/json")

SettingsOverlay = {
    buttonRadius = 6
}

function SettingsOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseSettingsOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "PauseSettingsCanvas")
    pauseSettingsOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, pauseSettingsOverlayCanvas.luaProxyId)
        pauseSettingsOverlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3,
            "Opacity", UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        pauseSettingsOverlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity", UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
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

    local soundLabel = UiLabel:new(host, "Lora-VariableFont_wght")
    pauseSettingsOverlay:addWidget(soundLabel)

    local musicLabel = UiLabel:new(host, "Lora-VariableFont_wght")
    pauseSettingsOverlay:addWidget(musicLabel)

    local musicSlider = UiSlider:new(host, "MusicSlider")
    pauseSettingsOverlay:addWidget(musicSlider)

    local soundSlider = UiSlider:new(host, "SoundSlider")
    pauseSettingsOverlay:addWidget(soundSlider)

    local applyButton = LabelButton:new(host, pauseSettingsOverlay, "Lora-VariableFont_wght", "ApplyButton")
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
    local cancelButton = LabelButton:new(host, pauseSettingsOverlay, "Lora-VariableFont_wght", "CancelButton")
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

        soundLabel:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName, 50)
        soundLabel:setHeight(buttonWidth / 4.0)
        soundLabel:setWidth(buttonWidth * 2.0)
        soundLabel:setText("Sound effects")
        soundLabel:setTextColorHexValue(0xFFFFFF)
        soundLabel:setFontSize(8.0)
        soundLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
        soundLabel:setZOrder(2)

        soundSlider:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundSlider:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin * 0.5)
        soundSlider:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            buttonHorizontalMargin * 0.5)
        soundSlider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, soundLabel.widgetName, 60.0)
        soundSlider:setHeight(30.0)
        soundSlider:setZOrder(2)
        soundSlider:setSliderValue(_GetSoundGain(host))
        soundSlider:setMaxSliderValue(1.0)
        soundSlider:setMinSliderValue(0.0)
        soundSlider:setSliderStep(0.1)
        soundSlider:setSliderThicknessPixels(10.0)
        soundSlider:setBlobThicknessPixels(40.0)
        soundSlider:setSliderType(UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL)
        soundSlider:setBlobColorHexValue(Styles.Colors.hoveredButtonColor)
        soundSlider:setSliderColorHexValue(Styles.Colors.notActiveButtonColor)
        soundSlider:enableSliderMouseInputReceiver(host)
        soundSlider:subscribeOnSliderValueChangedCallback(function(newValue)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                "GeneralSystemSettingsEvents", json.encode({
                    action = "change_sound",
                    gain = math.max(0.0, math.min(newValue, 1.0))
                }))
        end)

        musicLabel:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        musicLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin)
        musicLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, soundSlider.widgetName, 50)
        musicLabel:setHeight(buttonWidth / 4.0)
        musicLabel:setWidth(buttonWidth * 2.0)
        musicLabel:setText("Music")
        musicLabel:setTextColorHexValue(0xFFFFFF)
        musicLabel:setFontSize(8.0)
        musicLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
        musicLabel:setZOrder(2)

        musicSlider:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        musicSlider:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            buttonHorizontalMargin * 0.5)
        musicSlider:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            buttonHorizontalMargin * 0.5)
        musicSlider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, musicLabel.widgetName, 60.0)
        musicSlider:setHeight(30.0)
        musicSlider:setZOrder(2)
        musicSlider:setSliderValue(_GetMusicGain(host))
        musicSlider:setMaxSliderValue(1.0)
        musicSlider:setMinSliderValue(0.0)
        musicSlider:setSliderStep(0.1)
        musicSlider:setSliderThicknessPixels(10.0)
        musicSlider:setBlobThicknessPixels(40.0)
        musicSlider:setSliderType(UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL)
        musicSlider:setBlobColorHexValue(Styles.Colors.hoveredButtonColor)
        musicSlider:setSliderColorHexValue(Styles.Colors.notActiveButtonColor)
        musicSlider:enableSliderMouseInputReceiver(host)
        musicSlider:subscribeOnSliderValueChangedCallback(function(newValue)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                "GeneralSystemSettingsEvents", json.encode({
                    action = "change_music",
                    gain = math.max(0.0, math.min(newValue, 1.0))
                }))
        end)

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
        cancelButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            backgroundRect.widgetName, 50)
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

    pauseSettingsOverlay.onGameEventTriggered = function(eventName, jsonArgs)
    end
    pauseSettingsOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "GeneralSystemSettingsChanged" == eventName and pauseSettingsOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["settings_type"] ~= nil then
                local settings_type = tostring(parsedJson["settings_type"])
                local action = tostring(parsedJson["action"])
                if "change_value" == action then
                    local gain = tonumber(parsedJson["gain"])
                    if gain ~= nil then
                        if "sound" == settings_type then
                            soundSlider:setSliderValue(gain)
                        elseif "music" == settings_type then
                            musicSlider:setSliderValue(gain)
                        end
                    end
                end
            end
        end
    end

    return pauseSettingsOverlay
end

return SettingsOverlay
