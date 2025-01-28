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

local EventsHelper = require("Ui/Core/eventsHelper")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local LabelButton = require("Ui/Widgets/LabelButton")

MainMenuOverlay = {
    buttonColor = 0x403649,
    hoveredButtonColor = 0x201b24,
    buttonRadius = 6
}

function MainMenuOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local mainMenuOverlay_1Canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "MainMenuCanvas")
    mainMenuOverlay_1Canvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, mainMenuOverlay_1Canvas.luaProxyId)
        mainMenuOverlay_1Canvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        mainMenuOverlay_1Canvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local mainMenuOverlay_1 = UiOverlay:createOverlay(host, "MainMenuOverlay", mainMenuOverlay_1Canvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 15.0;

    local backgroundRect = UiRectangle:new(host, "MainMenuBackgroundContainer")
    mainMenuOverlay_1:addWidget(backgroundRect)

    local mainMenuOverlayHeight = (windowHeight - (menuVerticalMargin * 2.0))
    local buttonsCount = 4;
    local buttonsMarginCount = buttonsCount + 1;
    local buttonHeight = (mainMenuOverlayHeight / buttonsCount)
    local buttonVerticalMarginHeight = buttonHeight / 4.0;
    local totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
    buttonHeight = (mainMenuOverlayHeight - totalButtonMarginHeight) / buttonsCount;

    local newGameButton = LabelButton:new(host, mainMenuOverlay_1, "nimbus_mono", "NewGameButton")
    mainMenuOverlay_1:addCompoundWidget(newGameButton)
    newGameButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newGameButton:setButtonColorHexValue(MainMenuOverlay.hoveredButtonColor)
        else
            newGameButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        end
    end)
    newGameButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "CombatLevel")
    end)

    local editorLvlButton = LabelButton:new(host, mainMenuOverlay_1, "nimbus_mono", "EditorLvlButton")
    mainMenuOverlay_1:addCompoundWidget(editorLvlButton)
    editorLvlButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            editorLvlButton:setButtonColorHexValue(MainMenuOverlay.hoveredButtonColor)
        else
            editorLvlButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        end
    end)
    editorLvlButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendLoadLevelGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevel")
    end)

    local settingsButton = LabelButton:new(host, mainMenuOverlay_1, "nimbus_mono", "SettingsButton")
    mainMenuOverlay_1:addCompoundWidget(settingsButton)
    settingsButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            settingsButton:setButtonColorHexValue(MainMenuOverlay.hoveredButtonColor)
        else
            settingsButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        end
    end)

    local exitGameButton = LabelButton:new(host, mainMenuOverlay_1, "nimbus_mono", "ExitGameButton")
    mainMenuOverlay_1:addCompoundWidget(exitGameButton)
    exitGameButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendExitGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    exitGameButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitGameButton:setButtonColorHexValue(MainMenuOverlay.hoveredButtonColor)
        else
            exitGameButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        end
    end)

    mainMenuOverlay_1:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("mainMenuOverlay_1:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, mainMenuOverlay_1Canvas.widgetName, mainMenuOverlay_1Canvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            mainMenuOverlay_1Canvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            mainMenuOverlay_1Canvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            mainMenuOverlay_1Canvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            mainMenuOverlay_1Canvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(0x6C5B7B)
        backgroundRect:setZOrder(1)
        backgroundRect:setBorderRadius(MainMenuOverlay.buttonRadius)

        newGameButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            buttonVerticalMarginHeight)
        newGameButton:setHeight(buttonHeight)
        newGameButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        newGameButton:setZOrder(2)
        newGameButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        newGameButton:setLabelText("New Game")
        newGameButton:setLabelTextColorHexValue(0xFFFFFF)
        newGameButton:setLabelFontSize(20.0)
        newGameButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)

        editorLvlButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect
            .widgetName,
            20)
        editorLvlButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, newGameButton.widgetName,
            buttonVerticalMarginHeight)
        editorLvlButton:setHeight(buttonHeight)
        editorLvlButton:setZOrder(2)
        editorLvlButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        editorLvlButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        editorLvlButton:setLabelText("Go to editor")
        editorLvlButton:setLabelTextColorHexValue(0xFFFFFF)
        editorLvlButton:setLabelFontSize(20.0)
        editorLvlButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)

        settingsButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            backgroundRect.widgetName, 20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            editorLvlButton.widgetName, buttonVerticalMarginHeight)
        settingsButton:setHeight(buttonHeight)
        settingsButton:setZOrder(2)
        settingsButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        settingsButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        settingsButton:setLabelText("Settings")
        settingsButton:setLabelTextColorHexValue(0xFFFFFF)
        settingsButton:setLabelFontSize(20.0)
        settingsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)

        exitGameButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName, buttonVerticalMarginHeight)
        exitGameButton:setHeight(buttonHeight)
        exitGameButton:setZOrder(2)
        exitGameButton:setButtonColorHexValue(MainMenuOverlay.buttonColor)
        exitGameButton:setButtonBorderRadius(MainMenuOverlay.buttonRadius)
        exitGameButton:setLabelText("Exit game")
        exitGameButton:setLabelTextColorHexValue(0xFFFFFF)
        exitGameButton:setLabelFontSize(20.0)
        exitGameButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
    end)

    mainMenuOverlay_1.onGameEventTriggered = function(eventName, jsonArgs) end
    mainMenuOverlay_1.onEngineEventTriggered = function(eventName, jsonArgs) end

    return mainMenuOverlay_1;
end

return MainMenuOverlay
