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
local EngineInputReceiver = require("Ui/Core/engineInputReceiver")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local EventsHelper = require("Ui/Core/eventsHelper")
local PauseOverlay = require("Ui/Overlays/MenuNavigation/PauseOverlay")
local SettingsOverlay = require("Ui/Overlays/MenuNavigation/SettingsOverlay")
local PlayerHUDOverlay = require("Ui/Overlays/CombatLevelOverlays/PlayerHUDOverlay")
local LevelFailedOverlay = require("Ui/Overlays/CombatLevelOverlays/LevelFailedOverlay")
local json = require("Ui/Core/3rdparty/json")

GlobalContext = {
}

UiOverlays = {
}

UiBackgroundOverlays = {
}

local pressButtonCooldown = 0.0

local function onPressedKeyboardButtons(host, keyboardPressedKeyNames)
    if keyboardPressedKeyNames ~= nil then
        for _, value in pairs(keyboardPressedKeyNames) do
            if value == "Escape" then
                if pressButtonCooldown >= 0.5 then
                    pressButtonCooldown = 0.0
                    if "PauseMenuOverlay" == UiOverlayManager:getCurrentOverlayName(host) then
                        EventsHelper:sendPauseGameThreadEvent(host,
                            EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
                        UiOverlayManager:closeCurrentOverlay(host)
                        -- restore active background overlays
                        --UiOverlayManager:openBackgroundOverlay(host, "PlayerHUDOverlay")
                    else
                        EventsHelper:sendPauseGameThreadEvent(host,
                            EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
                        UiOverlayManager:openOverlay(host, "PauseMenuOverlay")
                        --store active background overlays
                        --UiOverlayManager:closeBackgroundOverlay(host, "PlayerHUDOverlay")
                    end
                end
            end
        end
    end
end

local function createPlayerHUDOverlay(host)
    return PlayerHUDOverlay:new(host)
end

local function createPauseOverlay(host)
    return PauseOverlay:new(host)
end

local function createPauseSettingsOverlay(host)
    return SettingsOverlay:new(host)
end

local function createLevelFailedOverlay(host)
    return LevelFailedOverlay:new(host)
end

local function initialize(host)
    UiOverlays["PauseSettingsOverlay"] = createPauseSettingsOverlay(host)
    UiOverlays["PauseMenuOverlay"] = createPauseOverlay(host)
    UiOverlays["LevelFailedOverlay"] = createLevelFailedOverlay(host)
    UiBackgroundOverlays["PlayerHUDOverlay"] = createPlayerHUDOverlay(host)
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

    for _, value in pairs(UiBackgroundOverlays) do
        value:updateFromReplicatorData(host)
    end

    for _, value in pairs(UiBackgroundOverlays) do
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

    for _, value in pairs(UiBackgroundOverlays) do
        value:sendDataToReplicator(host)
    end
end

function System_OnEngineEventTriggered(host, eventName, jsonArgs)
    assert(eventName ~= nil and type(eventName) == "string")

    for _, value in pairs(UiOverlays) do
        value.onEngineEventTriggered(eventName, jsonArgs)
    end
    for _, value in pairs(UiBackgroundOverlays) do
        value.onEngineEventTriggered(eventName, jsonArgs)
    end
end

function System_OnGameEventTriggered(host, eventName, jsonArgs)
    assert(eventName ~= nil and type(eventName) == "string")

    if "PlayerStatusChanged" == eventName then
        assert(jsonArgs ~= nil and type(jsonArgs) == "string")
        local parsedJson = json.decode(jsonArgs)
        if parsedJson["player_status_type"] ~= nil then
            local statusType = tonumber(parsedJson["player_status_type"])
            if statusType == PlayerStatusType.SELECTED_TOWER_CHANGED then
                if parsedJson["has_selected_tower"] ~= nil then
                    if parsedJson["has_selected_tower"] == true then
                        UiOverlayManager:openBackgroundOverlay(host, "PlayerHUDOverlay")
                    else
                        UiOverlayManager:closeBackgroundOverlay(host, "PlayerHUDOverlay")
                    end
                end
            end
        end
    end

    for _, value in pairs(UiOverlays) do
        value.onGameEventTriggered(eventName, jsonArgs)
    end
    for _, value in pairs(UiBackgroundOverlays) do
        value.onGameEventTriggered(eventName, jsonArgs)
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
