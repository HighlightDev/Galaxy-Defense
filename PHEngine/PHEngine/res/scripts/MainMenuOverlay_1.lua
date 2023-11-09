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

MainMenuOverlay_1 = {
    buttonColor = 0x403649,
    hoveredButtonColor = 0x201b24,
    buttonRadius = 6
}

function MainMenuOverlay_1:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local mainMenuOverlay_1Canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    mainMenuOverlay_1Canvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, mainMenuOverlay_1Canvas.luaProxyId)
        mainMenuOverlay_1Canvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        mainMenuOverlay_1Canvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
            0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local mainMenuOverlay_1 = UiOverlay:createOverlay(host, "MainMenuOverlay_1", mainMenuOverlay_1Canvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 15.0;

    local backgroundRect = UiRectangle:new(host)
    mainMenuOverlay_1:addWidget(backgroundRect)

    local mainMenuOverlayHeight = (windowHeight - (menuVerticalMargin * 2.0))
    local buttonsCount = 4;
    local buttonsMarginCount = buttonsCount + 1;
    local buttonHeight = (mainMenuOverlayHeight / buttonsCount)
    local buttonVerticalMarginHeight = buttonHeight / 4.0;
    local totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
    buttonHeight = (mainMenuOverlayHeight - totalButtonMarginHeight) / buttonsCount;

    local newGameButton = UiRectangle:new(host)
    mainMenuOverlay_1:addWidget(newGameButton)
    newGameButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newGameButton:setColorHexValue(MainMenuOverlay_1.hoveredButtonColor)
        else
            newGameButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        end
    end)
    newGameButton:setOnMouseInputClickedCallback(function()
        EngineEventsHolder:sendLoadLevelGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "FirstLevel")
    end)

    local newGameButtonLabel = UiLabel:new(host, "nimbus_mono")
    mainMenuOverlay_1:addWidget(newGameButtonLabel)

    local selectLvlButton = UiRectangle:new(host)
    mainMenuOverlay_1:addWidget(selectLvlButton)
    selectLvlButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            selectLvlButton:setColorHexValue(MainMenuOverlay_1.hoveredButtonColor)
        else
            selectLvlButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        end
    end)

    local selectLvlButtonLabel = UiLabel:new(host, "nimbus_mono")
    mainMenuOverlay_1:addWidget(selectLvlButtonLabel)

    local settingsButton = UiRectangle:new(host)
    mainMenuOverlay_1:addWidget(settingsButton)
    settingsButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            settingsButton:setColorHexValue(MainMenuOverlay_1.hoveredButtonColor)
        else
            settingsButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        end
    end)

    local settingsButtonLabel = UiLabel:new(host, "nimbus_mono")
    mainMenuOverlay_1:addWidget(settingsButtonLabel)

    local exitGameButton = UiRectangle:new(host)
    mainMenuOverlay_1:addWidget(exitGameButton)
    exitGameButton:setOnMouseInputClickedCallback(function()
        EngineEventsHolder:sendExitGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH)
    end)
    exitGameButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitGameButton:setColorHexValue(MainMenuOverlay_1.hoveredButtonColor)
        else
            exitGameButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        end
    end)

    local exitGameMenuButtonLabel = UiLabel:new(host, "nimbus_mono")
    mainMenuOverlay_1:addWidget(exitGameMenuButtonLabel)

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
        backgroundRect:setBorderRadius(MainMenuOverlay_1.buttonRadius)

        newGameButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        newGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            buttonVerticalMarginHeight)
        newGameButton:setHeight(buttonHeight)
        newGameButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        newGameButton:setZOrder(2)
        newGameButton:enableMouseInputReceiverBase(host)
        newGameButton:setBorderRadius(MainMenuOverlay_1.buttonRadius)

        newGameButtonLabel:setParent(host, mainMenuOverlay_1Canvas.widgetName, newGameButton.widgetName)
        newGameButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            newGameButton.widgetName)
        newGameButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            newGameButton.widgetName)
        newGameButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, newGameButton
            .widgetName)
        newGameButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            newGameButton.widgetName)
        newGameButtonLabel:setText("New Game")
        newGameButtonLabel:setTextColorHexValue(0xFFFFFF)
        newGameButtonLabel:setFontSize(20.0)
        newGameButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        newGameButtonLabel:setZOrder(3)

        selectLvlButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        selectLvlButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        selectLvlButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect
            .widgetName,
            20)
        selectLvlButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, newGameButton.widgetName,
            buttonVerticalMarginHeight)
        selectLvlButton:setHeight(buttonHeight)
        selectLvlButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        selectLvlButton:setZOrder(2)
        selectLvlButton:enableMouseInputReceiverBase(host)
        selectLvlButton:setBorderRadius(MainMenuOverlay_1.buttonRadius)

        selectLvlButtonLabel:setParent(host, mainMenuOverlay_1Canvas.widgetName, selectLvlButton.widgetName)
        selectLvlButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            selectLvlButton.widgetName)
        selectLvlButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            selectLvlButton.widgetName)
        selectLvlButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, selectLvlButton
            .widgetName)
        selectLvlButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            selectLvlButton.widgetName)
        selectLvlButtonLabel:setText("Select level")
        selectLvlButtonLabel:setTextColorHexValue(0xFFFFFF)
        selectLvlButtonLabel:setFontSize(20.0)
        selectLvlButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        selectLvlButtonLabel:setZOrder(3)

        settingsButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            backgroundRect.widgetName, 20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, 20)
        settingsButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            selectLvlButton.widgetName, buttonVerticalMarginHeight)
        settingsButton:setHeight(buttonHeight)
        settingsButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        settingsButton:setZOrder(2)
        settingsButton:enableMouseInputReceiverBase(host)
        settingsButton:setBorderRadius(MainMenuOverlay_1.buttonRadius)

        settingsButtonLabel:setParent(host, mainMenuOverlay_1Canvas.widgetName, settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            settingsButton.widgetName)
        settingsButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName)
        settingsButtonLabel:setText("Settings")
        settingsButtonLabel:setTextColorHexValue(0xFFFFFF)
        settingsButtonLabel:setFontSize(20.0)
        settingsButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        settingsButtonLabel:setZOrder(3)

        exitGameButton:setParent(host, mainMenuOverlay_1Canvas.widgetName, backgroundRect.widgetName)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        exitGameButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
            settingsButton.widgetName, buttonVerticalMarginHeight)
        exitGameButton:setHeight(buttonHeight)
        exitGameButton:setColorHexValue(MainMenuOverlay_1.buttonColor)
        exitGameButton:setZOrder(2)
        exitGameButton:enableMouseInputReceiverBase(host)
        exitGameButton:setBorderRadius(MainMenuOverlay_1.buttonRadius)

        exitGameMenuButtonLabel:setParent(host, mainMenuOverlay_1Canvas.widgetName, exitGameButton.widgetName)
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

    mainMenuOverlay_1.onGameEventTriggered = function(eventName, jsonArgs) end
    mainMenuOverlay_1.onEngineEventTriggered = function(eventName, jsonArgs) end

    return mainMenuOverlay_1;
end

return MainMenuOverlay_1
