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
local EngineInputReceiver = require("core/engineInputReceiver")
local UiOverlayManager = require("core/uiOverlayManager")
local EngineEventsHolder = require("core/engineEventsHolder")
local UiCanvas = require("core/uiCanvas")
local UiOverlay = require("core/uiOverlay")
local UiItem = require("core/uiItem")
local UiRectangle = require("core/uiRectangle")
local UiLabel = require("core/uiLabel")
local UiToggleButton = require("core/uiToggleButton")

GlobalContext = {
}

UiOverlays = {
}

local pressButtonCooldown = 0.0

local function onPressedKeyboardButtons(host, keyboardPressedKeyNames)
    if keyboardPressedKeyNames ~= nil then
        for _, value in pairs(keyboardPressedKeyNames) do
            if value == "Escape" then
                if pressButtonCooldown >= 0.5 then
                    pressButtonCooldown = 0.0
                    if "PauseMenuOverlay" == UiOverlayManager:getCurrentOverlayName(host) then
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
                        UiOverlayManager:closeCurrentOverlay(host)
                    else
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
                        UiOverlayManager:openOverlay(host, "PauseMenuOverlay")
                    end
                end
            end
        end
    end
end

local s_buttonColor = 0x403649
local s_hoveredButtonColor = 0x201b24

local function createPauseOverlay(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseMenuOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    pauseMenuOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, pauseMenuOverlayCanvas.luaProxyId)
    end)
    local pauseMenuOverlay = UiOverlay:new(host, "PauseMenuOverlay", pauseMenuOverlayCanvas)

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
    end)
    continueButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            continueButton:setColorHexValue(s_hoveredButtonColor)
        else
            continueButton:setColorHexValue(s_buttonColor)
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
            settingsButton:setColorHexValue(s_hoveredButtonColor)
        else
            settingsButton:setColorHexValue(s_buttonColor)
        end
    end)

    local settingsButtonLabel = UiLabel:new(host, "nimbus_mono")
    pauseMenuOverlay:addWidget(settingsButtonLabel)

    local exitToMainMenuButton = UiRectangle:new(host)
    pauseMenuOverlay:addWidget(exitToMainMenuButton)
    exitToMainMenuButton:setOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            exitToMainMenuButton:setColorHexValue(s_hoveredButtonColor)
        else
            exitToMainMenuButton:setColorHexValue(s_buttonColor)
        end
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
            exitGameButton:setColorHexValue(s_hoveredButtonColor)
        else
            exitGameButton:setColorHexValue(s_buttonColor)
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

        continueButton:setParent(host, pauseMenuOverlayCanvas.widgetName, backgroundRect.widgetName)
        continueButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName,
            20)
        continueButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName,
            20)
        continueButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            buttonVerticalMarginHeight)
        continueButton:setHeight(buttonHeight)
        continueButton:setColorHexValue(s_buttonColor)
        continueButton:setZOrder(2)
        continueButton:enableMouseInputReceiverBase(host)

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
        settingsButton:setColorHexValue(s_buttonColor)
        settingsButton:setZOrder(2)
        settingsButton:enableMouseInputReceiverBase(host)

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
        exitToMainMenuButton:setColorHexValue(s_buttonColor)
        exitToMainMenuButton:setZOrder(2)
        exitToMainMenuButton:enableMouseInputReceiverBase(host)

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
        exitGameButton:setColorHexValue(s_buttonColor)
        exitGameButton:setZOrder(2)
        exitGameButton:enableMouseInputReceiverBase(host)

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

    return pauseMenuOverlay;
end

local function createPauseSettingsOverlay(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local pauseSettingsOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    pauseSettingsOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, pauseSettingsOverlayCanvas.luaProxyId)
    end)
    local pauseSettingsOverlay = UiOverlay:new(host, "PauseSettingsOverlay", pauseSettingsOverlayCanvas)

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
            applyButton:setColorHexValue(s_hoveredButtonColor)
        else
            applyButton:setColorHexValue(s_buttonColor)
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
            cancelButton:setColorHexValue(s_hoveredButtonColor)
        else
            cancelButton:setColorHexValue(s_buttonColor)
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

        soundToggleButton:setParent(host, pauseSettingsOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            backgroundRect.widgetName, buttonHorizontalMargin)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
            50)
        soundToggleButton:setWidth(buttonWidth / 4.0)
        soundToggleButton:setHeight(buttonWidth / 4.0)
        soundToggleButton:setZOrder(2)
        soundToggleButton:setToggleOnColorHexValue(0xFFB732)
        soundToggleButton:setToggleOffColorHexValue(s_buttonColor)
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
        applyButton:setColorHexValue(s_buttonColor)
        applyButton:setZOrder(2)
        applyButton:enableMouseInputReceiverBase(host)

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
        cancelButton:setColorHexValue(s_buttonColor)
        cancelButton:setZOrder(2)
        cancelButton:enableMouseInputReceiverBase(host)

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

    return pauseSettingsOverlay
end

local function initialize(host)
    UiOverlays["PauseSettingsOverlay"] = createPauseSettingsOverlay(host)
    UiOverlays["PauseMenuOverlay"] = createPauseOverlay(host)
end

function System_OnStart(host)
    local engineReceiver = EngineInputReceiver:new()
    engineReceiver.subscribeToMouseEvents = false
    engineReceiver:subscribeOnPressedKeyboardButton(onPressedKeyboardButtons)
    GlobalContext["inputReceiver"] = engineReceiver

    initialize(host)
end

function System_OnUpdate(host, deltaTimeSec)
    if math.abs(pressButtonCooldown) > 1 then
        pressButtonCooldown = 0
    end

    pressButtonCooldown = pressButtonCooldown + deltaTimeSec

    for _, value in pairs(UiOverlays) do
        value:updateFromReplicatorData(host)
    end

    for _, value in pairs(UiOverlays) do
        value:update(host, deltaTimeSec)
    end

    for _, value in pairs(GlobalContext) do
        if value.canUpdate then
            value:update(host)
        end
    end

    for _, value in pairs(UiOverlays) do
        value:sendDataToReplicator(host)
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
