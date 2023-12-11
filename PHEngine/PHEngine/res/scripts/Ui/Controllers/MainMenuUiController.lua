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
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local MainMenuOverlay_1 = require("Ui/Overlays/MainMenuOverlay_1")

GlobalContext = {
}

UiOverlays = {
}

local function createMainMenuOverlay(host, overlayNumber)
    assert(host ~= nil and type(host) == "userdata" and overlayNumber ~= nil and type(overlayNumber) == "number" and
        overlayNumber > 0 and overlayNumber <= 1)
    if overlayNumber == 1 then
        return MainMenuOverlay_1:new(host)
    end
    return nil;
end

local function initialize(host)
    UiOverlays["MainMenuOverlay_1"] = createMainMenuOverlay(host, 1)
end

function System_OnStart(host)
    initialize(host)
    UiOverlayManager:openOverlay(host, "MainMenuOverlay_1")
end

function System_OnUpdate(host, deltaTimeSec)

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

function System_OnEngineEventTriggered(host, eventName, jsonArgs)
    assert(eventName ~= nil and type(eventName) == "string")

    for _, value in pairs(UiOverlays) do
        value.onEngineEventTriggered(eventName, jsonArgs)
    end
end

function System_OnGameEventTriggered(host, eventName, jsonArgs)
    assert(eventName ~= nil and type(eventName) == "string")

    for _, value in pairs(UiOverlays) do
        value.onGameEventTriggered(eventName, jsonArgs)
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
