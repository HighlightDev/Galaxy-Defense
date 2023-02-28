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

GlobalContext = {
}

local function addToGlobalContext(key, object)
    GlobalContext[key] = object
end

local function onPressedKeyboardButtons(host, keyboardPressedKeyNames)
    if keyboardPressedKeyNames ~= nil then
        for _, value in pairs(keyboardPressedKeyNames) do
            if value == "Escape" then
                print("Overlay opened: " .. tostring(UiOverlayManager:getCurrentOverlayName(host)))
                break
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

function System_OnUpdate(host)
    for _, value in pairs(GlobalContext) do
        if value.canUpdate then
            value:update(host)
        end
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
