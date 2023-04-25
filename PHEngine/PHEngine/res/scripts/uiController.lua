--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
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
            if value == "L" then
                UiOverlayManager:openOverlay(host, "TestOverlay")
            end
            if value == "Escape" then
                if pressButtonCooldown >= 0.5 then
                    pressButtonCooldown = 0.0
                    if "PauseMenu" == UiOverlayManager:getCurrentOverlayName(host) then
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
                        UiOverlayManager:closeCurrentOverlay(host)
                    else
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
                        UiOverlayManager:openOverlay(host, "PauseMenu");
                    end
                end
            end
        end
    end
end

local function createTestOverlay(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local testOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    testOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, testOverlayCanvas.luaProxyId)
    end)
    local testOverlay = UiOverlay:new(host, "TestOverlay", testOverlayCanvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 7.0;

    local rowButtonsCount = 2.0;
    local backgroundRectWidth = windowWidth - (menuHorizontalMargin * 2.0);
    local buttonHorizontalMargin = backgroundRectWidth / 10.0;
    local buttonWidth = (backgroundRectWidth - (buttonHorizontalMargin * (rowButtonsCount + 1.0))) / rowButtonsCount;

    local backgroundRect = UiRectangle:new(host)
    testOverlay:addWidget(backgroundRect)

    local soundToggleButton = UiToggleButton:new(host, false)
    testOverlay:addWidget(soundToggleButton)

    local soundLabel = UiLabel:new(host, "nimbus_mono")
    testOverlay:addWidget(soundLabel)

    local applyButton = UiRectangle:new(host)
    testOverlay:addWidget(applyButton)

    local applyButtonLabel = UiLabel:new(host, "nimbus_mono")
    testOverlay:addWidget(applyButtonLabel)

    local cancelButton = UiRectangle:new(host)
    testOverlay:addWidget(cancelButton)

    local cancelButtonLabel = UiLabel:new(host, "nimbus_mono")
    testOverlay:addWidget(cancelButtonLabel)

    testOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("testOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, testOverlayCanvas.widgetName, testOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, testOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, testOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, testOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, testOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(0x6C5B7B)
        backgroundRect:setZOrder(1)

        soundToggleButton:setParent(host, testOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName, buttonHorizontalMargin)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName, 50)
        soundToggleButton:setWidth(buttonWidth / 4.0)
        soundToggleButton:setHeight(buttonWidth / 4.0)
        soundToggleButton:setZOrder(2)
        soundToggleButton:setToggleOnColorHexValue(0xFFB732)
        soundToggleButton:setToggleOffColorHexValue(0x403649)

        soundLabel:setParent(host, testOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName, buttonHorizontalMargin)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.LEFT, soundToggleButton.widgetName, buttonHorizontalMargin)
        soundLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName, 50)
        soundLabel:setHeight(buttonWidth / 4.0);
        soundLabel:setText("Enable sound effects");
        soundLabel:setTextColorHexValue(0xFFFFFF);
        soundLabel:setFontSize(11.0);
        soundLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT);
        soundLabel:setZOrder(2);

        applyButton:setParent(host, testOverlayCanvas.widgetName, backgroundRect.widgetName);
        applyButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, backgroundRect.widgetName, buttonHorizontalMargin);
        applyButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName, 50);
        applyButton:setWidth(buttonWidth);
        applyButton:setHeight(100);
        applyButton:setColorHexValue(0x403649);
        applyButton:setZOrder(2);

        applyButtonLabel:setParent(host, testOverlayCanvas.widgetName, applyButton.widgetName);
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, applyButton.widgetName, 0);
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, applyButton.widgetName, 0);
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, applyButton.widgetName, 0);
        applyButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, applyButton.widgetName, 0);
        applyButtonLabel:setText("Apply");
        applyButtonLabel:setTextColorHexValue(0xFFFFFF);
        applyButtonLabel:setFontSize(20.0);
        applyButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER);
        applyButtonLabel:setZOrder(3);

        cancelButton:setParent(host, testOverlayCanvas.widgetName, backgroundRect.widgetName);
        cancelButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, applyButton.widgetName, buttonHorizontalMargin);
        cancelButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, backgroundRect.widgetName, 50);
        cancelButton:setWidth(buttonWidth);
        cancelButton:setHeight(100);
        cancelButton:setColorHexValue(0x403649);
        cancelButton:setZOrder(2);

        cancelButtonLabel:setParent(host, testOverlayCanvas.widgetName, cancelButton.widgetName);
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, cancelButton.widgetName, 0);
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, cancelButton.widgetName, 0);
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, cancelButton.widgetName, 0);
        cancelButtonLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, cancelButton.widgetName, 0);
        cancelButtonLabel:setText("Cancel");
        cancelButtonLabel:setTextColorHexValue(0xFFFFFF);
        cancelButtonLabel:setFontSize(20.0);
        cancelButtonLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER);
        cancelButtonLabel:setZOrder(3);
    end)

    return testOverlay
end

local function initialize(host)
    UiOverlays["TestOverlay"] = createTestOverlay(host)
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
