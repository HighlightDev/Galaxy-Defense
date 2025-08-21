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

local EventsHelper = require("Ui/Core/eventsHelper")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")

MainMenuOverlay = {
    buttonRadius = 6
}

function MainMenuOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local mainMenuOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "MainMenuCanvas")
    mainMenuOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, mainMenuOverlayCanvas.luaProxyId)
        mainMenuOverlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3,
            "Opacity", UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        mainMenuOverlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3,
            "Opacity", UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local mainMenuOverlay = UiOverlay:createOverlay(host, "MainMenuOverlay", mainMenuOverlayCanvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 15.0;

    local backgroundRect = UiRectangle:new(host, "MainMenuBackgroundContainer")
    mainMenuOverlay:addWidget(backgroundRect)

    local mainMenuOverlayHeight = (windowHeight - (menuVerticalMargin * 2.0))
    local buttonsCount = 4;
    local buttonsMarginCount = buttonsCount + 1;
    local buttonHeight = (mainMenuOverlayHeight / buttonsCount)
    local buttonVerticalMarginHeight = buttonHeight / 4.0;
    local totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
    buttonHeight = (mainMenuOverlayHeight - totalButtonMarginHeight) / buttonsCount;

    local newGameButton = LabelButton:new(host, mainMenuOverlay, "Lora-VariableFont_wght", "NewGameButton")
    mainMenuOverlay:addCompoundWidget(newGameButton)
    newGameButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newGameButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            newGameButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)
    newGameButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "CombatLevel")
    end)

    local editorLvlButton = LabelButton:new(host, mainMenuOverlay, "Lora-VariableFont_wght", "EditorLvlButton")
    mainMenuOverlay:addCompoundWidget(editorLvlButton)
    editorLvlButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            editorLvlButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            editorLvlButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)
    editorLvlButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevel")
    end)

    local settingsButton = LabelButton:new(host, mainMenuOverlay, "Lora-VariableFont_wght", "SettingsButton")
    mainMenuOverlay:addCompoundWidget(settingsButton)
    settingsButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            settingsButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            settingsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local exitGameButton = LabelButton:new(host, mainMenuOverlay, "Lora-VariableFont_wght", "ExitGameButton")
    mainMenuOverlay:addCompoundWidget(exitGameButton)
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

    mainMenuOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("mainMenuOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, mainMenuOverlayCanvas.widgetName, mainMenuOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            mainMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            mainMenuOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            mainMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            mainMenuOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
        backgroundRect:setZOrder(1)
        backgroundRect:setBorderRadius(MainMenuOverlay.buttonRadius)

        newGameButton:setParent(host, mainMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            buttonVerticalMarginHeight)
        newGameButton:setHeight(buttonHeight)
        newGameButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        newGameButton:setZOrder(2)
        newGameButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        newGameButton:setLabelText("New Game")
        newGameButton:setLabelTextColorHexValue(0xFFFFFF)
        newGameButton:setLabelFontSize(56)
        newGameButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        newGameButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)

        editorLvlButton:setParent(host, mainMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, newGameButton.widgetName,
            buttonVerticalMarginHeight)
        editorLvlButton:setHeight(buttonHeight)
        editorLvlButton:setZOrder(2)
        editorLvlButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        editorLvlButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        editorLvlButton:setLabelText("Go to editor")
        editorLvlButton:setLabelTextColorHexValue(0xFFFFFF)
        editorLvlButton:setLabelFontSize(56)
        editorLvlButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editorLvlButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)

        settingsButton:setParent(host, mainMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            editorLvlButton.widgetName, buttonVerticalMarginHeight)
        settingsButton:setHeight(buttonHeight)
        settingsButton:setZOrder(2)
        settingsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        settingsButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        settingsButton:setLabelText("Settings")
        settingsButton:setLabelTextColorHexValue(0xFFFFFF)
        settingsButton:setLabelFontSize(56)
        settingsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        settingsButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)

        exitGameButton:setParent(host, mainMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, settingsButton.widgetName,
            buttonVerticalMarginHeight)
        exitGameButton:setHeight(buttonHeight)
        exitGameButton:setZOrder(2)
        exitGameButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        exitGameButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        exitGameButton:setLabelText("Exit game")
        exitGameButton:setLabelTextColorHexValue(0xFFFFFF)
        exitGameButton:setLabelFontSize(56)
        exitGameButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        exitGameButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    end)

    mainMenuOverlay.onGameEventTriggered = function(eventName, jsonArgs)
    end
    mainMenuOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
    end

    return mainMenuOverlay;
end

return MainMenuOverlay
