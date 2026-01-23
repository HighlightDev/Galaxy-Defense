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

local EngineInputReceiver = require("Ui/Core/engineInputReceiver")

local particlesLuaProxyId = nil
local engineReceiver = nil

local function onPressedKeyboardButtons(host, pressedKeyNames)
    for _, keyName in pairs(pressedKeyNames) do
        if keyName == "F" then _InvokeFunction(host, particlesLuaProxyId, "EmitParticles", "") end
    end
end

function System_OnStart(host)
    print("Billboard Rotation Script Started")
    particlesLuaProxyId = _GetLuaProxyIdForComponent(host, "c_particleSystem")
    assert(particlesLuaProxyId ~= nil, "Particle system Lua proxy ID is nil")

    engineReceiver = EngineInputReceiver:new()
    engineReceiver.subscribeToMouseEvents = false
    engineReceiver:subscribeOnPressedKeyboardButton(onPressedKeyboardButtons)
end

function System_OnUpdate(host, deltaTime) if engineReceiver ~= nil then engineReceiver:update(host) end end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
