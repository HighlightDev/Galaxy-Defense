--[[ BEGIN *** this snippet have to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--[[ END   *** this snippet have to be inserted everywhere where your want to require custom modules  ***  END]]
--

local EngineInputReceiver = require("core/engineInputReceiver")
local UiOverlayManager = require("core/uiOverlayManager")
local EngineEventsHolder = require("core/engineEventsHolder")

GlobalContext = {
}

local function addToGlobalContext(key, object)
    GlobalContext[key] = object
end

local pressButtonCooldown = 0.0

local function onPressedKeyboardButtons(host, keyboardPressedKeyNames)
    if keyboardPressedKeyNames ~= nil then
        for _, value in pairs(keyboardPressedKeyNames) do
            if value == "Escape" then
                if pressButtonCooldown >= 0.5 then
                    pressButtonCooldown = 0.0
                    if "PauseMenu" == UiOverlayManager:getCurrentOverlayName(host) then
                        EngineEventsHolder:sendPauseGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
                        UiOverlayManager:closeCurrentOverlay(host)
                    else
                        EngineEventsHolder:sendPauseGameThreadEvent(host, EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
                        UiOverlayManager:openOverlay(host, "PauseMenu");
                    end
                end
            end
        end
    end
end

function System_OnStart(host)
    local obj = EngineInputReceiver:new()
    obj.subscribeToMouseEvents = false
    obj:subscribeOnPressedKeyboardButton(onPressedKeyboardButtons)

    addToGlobalContext("testInpuReceiver", obj)
end

function System_OnUpdate(host, deltaTimeSec)
    for _, value in pairs(GlobalContext) do
        if value.canUpdate then
            value:update(host)
        end
    end
    pressButtonCooldown = pressButtonCooldown + deltaTimeSec
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
