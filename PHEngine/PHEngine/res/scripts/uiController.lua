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

GlobalContext = {
}

local function addToGlobalContext(key, object)
    print("addToGlobalContext => " .. tostring(object))
    GlobalContext[key] = object
end

function System_OnStart(host)
    print("System_OnStart")
    local obj = EngineInputReceiver:new()
    obj.subscribeToMouseEvents = false
    obj:subscribeOnPressedKeyboardButton(function()
        print("Keyboard buttons pressed!")
    end)

    addToGlobalContext("testInpuReceiver", obj)
end

function System_OnUpdate(host)
    --print("System_OnUpdate")
    for _, value in pairs(GlobalContext) do
        if value.canUpdate then
            value:update(host)
        end
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
