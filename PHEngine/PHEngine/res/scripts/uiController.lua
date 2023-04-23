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
    TestOverlayCanvas = UiCanvas:new(host, 0, 0, 900, 900)
    TestOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, TestOverlayCanvas.luaProxyId)
    end)

    TestRectangle = UiRectangle:new(host)
    TestRectangle:subscribeOnLuaProxyReady(function(host)
        print("UiRectangle::luaProxyReady => name: " ..
            tostring(TestRectangle.widgetName) .. ", id: " .. tostring(TestRectangle.luaProxyId))
    end)

    TestOverlay = UiOverlay:new(host, "TestOverlay", TestOverlayCanvas)
    TestOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("TestOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        TestRectangle:setParent(host, TestOverlayCanvas.widgetName, TestOverlayCanvas.widgetName)

        TestRectangle:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            TestOverlayCanvas.widgetName, 0)
        TestRectangle:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            TestOverlayCanvas.widgetName, 0)
        TestRectangle:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            TestOverlayCanvas.widgetName, 0)
        TestRectangle:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            TestOverlayCanvas.widgetName, 0)

        TestRectangle:setColorHexValue(0xAFFFFF)
        TestRectangle:setZOrder(1)
    end)
    TestOverlay:addWidget(TestRectangle)

    return TestOverlay
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
