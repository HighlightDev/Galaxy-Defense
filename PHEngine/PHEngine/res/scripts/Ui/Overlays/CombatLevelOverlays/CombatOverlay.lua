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
        package.path = package.path .. "" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiItem = require("Ui/Core/uiItem")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")
local UiRowLayout = require("Ui/Core/uiRowLayout")
local WeaponTile = require("Ui/Widgets/WeaponTile")
local ImageAndLabelTile = require("Ui/Widgets/ImageAndLabelTile")

LevelProgressStatusType = {
    NONE = 0,
    CURRENT_STAGE_CHANGED = 1,
    REQUIREMENT_TRACKERS_STATUS_CHANGED = 2
}

CombatOverlay = {
    levelProgressContainer = nil,
    levelProgressRowLayout = nil
}

RequirementTrackers = {}
RequirementTrackersIdle = {}

local function showTileRequirementAchived(requirementTile)
    requirementTile:setTextureSource("check.png")
    requirementTile:setLabelVisibility(false)
    requirementTile:setFlipImage(true)
    requirementTile:setImageColorHexValue(0xFFFFFF)
    requirementTile:setUseImageCustomColor(true)
end

local function showTileRequirementFailed(requirementTile)
    requirementTile:setTextureSource("cancel.png")
    requirementTile:setLabelVisibility(false)
    requirementTile:setImageColorHexValue(0xFFFFFF)
    requirementTile:setUseImageCustomColor(true)
end

local function showAllRequirementsAchived()
    for _, requirementTile in pairs(RequirementTrackers) do
        showTileRequirementAchived(requirementTile)
    end
end

local function updateRequirementTileData(host, levelProgressTile, trackerJson)
    local name = trackerJson["name"]
    if name == "DestroySpaceshipsTracker" then
        local leftSpaceshipsCount = trackerJson["left_to_destroy_spaceships_count"]
        if tonumber(leftSpaceshipsCount) > 0 then
            levelProgressTile:setLabelText(leftSpaceshipsCount)
        else
            showTileRequirementAchived(levelProgressTile)
        end
    elseif name == "MissedSpaceshipsTracker" then
        local doNotMissCount = tonumber(trackerJson["not_to_miss_spaceships_count"])
        local missedCount = tonumber(trackerJson["missed_spaceships_count"])
        local stillCanMiss = math.max(doNotMissCount - missedCount, 0)
        if stillCanMiss > 0 then
            levelProgressTile:setLabelText(tostring(stillCanMiss))
        else
            showTileRequirementFailed(levelProgressTile)
            UiOverlayManager:openOverlay(host, "LevelFailedOverlay")
            EventsHelper:sendPauseGameThreadEvent(host,
                EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        end
    else
        assert(false, "Not supported requirement: " .. name)
    end
end

local function fillRequirementTilesPool(host, combatOverlay, count)
    for i = 1, count, 1 do
        local levelProgressTile = ImageAndLabelTile:new(host, combatOverlay)
        combatOverlay:addCompoundWidget(levelProgressTile)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] = levelProgressTile

        levelProgressTile:subscribeOnLuaProxiesReady(function(host)
            levelProgressTile:setParent(host, combatOverlay:getOverlayCanvas().widgetName,
                combatOverlay.levelProgressRowLayout.widgetName)
            local tileSize = 100 -- temporary for now
            levelProgressTile:setWidth(tileSize)
            levelProgressTile:setHeight(tileSize)
            levelProgressTile:setTextureSource("space_station_img.png")
            levelProgressTile:setBackgroundTileOpacity(1.0)
            levelProgressTile:setIsVisible(false)
        end)
    end
end

local function updateRequirementTiles(host, combatOverlay, requirementTrackers)
    -- move active requirements tiles to idle state
    for _, activeTrackerTile in pairs(RequirementTrackers) do
        activeTrackerTile:setIsVisible(false)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] = activeTrackerTile
    end
    RequirementTrackers = {}

    local action = function()
        for index = 1, #requirementTrackers, 1 do
            local trackerTile = RequirementTrackersIdle[index]
            table.remove(RequirementTrackersIdle, index)
            RequirementTrackers[index] = trackerTile
            local trackerJson = requirementTrackers[index]
            updateRequirementTileData(host, trackerTile, trackerJson)
            trackerTile:setIsVisible(true)
        end
    end

    local predicate = function()
        return combatOverlay.allWidgetLuaProxiesReady
    end

    if predicate() then
        action()
    else
        combatOverlay:addActionWithPredicate(action, predicate)
    end
end

function CombatOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local combatOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    combatOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, combatOverlayCanvas.luaProxyId)
    end)
    local combatOverlay = UiOverlay:createBackgroundOverlay(host, "CombatOverlay", combatOverlayCanvas)

    local lifeRootContainerWidth = windowWidth / 3.0
    local lifeRootContainerHeight = windowHeight / 4.0
    local weaponRootContainerWidth = windowWidth / 5.0
    local heartWidth = lifeRootContainerWidth / 10.0
    local heartInterval = heartWidth * 0.5

    local levelProgressContainer = UiItem:new(host, "LvlProgressContainer")
    combatOverlay:addWidget(levelProgressContainer)
    combatOverlay.levelProgressContainer = levelProgressContainer

    local levelProgressRowLayout = UiRowLayout:new(host, "LvlProgressRow")
    combatOverlay:addWidget(levelProgressRowLayout)
    combatOverlay.levelProgressRowLayout = levelProgressRowLayout

    fillRequirementTilesPool(host, combatOverlay, 10)

    combatOverlay.onCurrentLevelProgressStageChanged = function()
        local currentProgressRequirementsCount = _GetCurrentProgressRequirementsCount(host)
        print("CombatOverlay::onCurrentLevelProgressStageChanged : req count: " ..
            tostring(currentProgressRequirementsCount))

        if currentProgressRequirementsCount > 0 then
            local requirementTrackersJson = _GetCurrentProgressStageRequirementTrackers(host)
            if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
                local parsedJson = json.decode(requirementTrackersJson)
                updateRequirementTiles(host, combatOverlay, parsedJson)
            end
        elseif #RequirementTrackers > 0 then
            showAllRequirementsAchived()
        end
    end

    combatOverlay.onRequirementTrackersStatusChanged = function()
        local requirementTrackersJson = _GetCurrentProgressStageRequirementTrackers(host)
        if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
            local parsedJson = json.decode(requirementTrackersJson)
            if parsedJson ~= nil and parsedJson ~= "" then
                assert(#parsedJson == #RequirementTrackers)
                for index, trackerJsonRoot in pairs(parsedJson) do
                    local requirementTrackerTile = RequirementTrackers[index]
                    assert(requirementTrackerTile ~= nil)
                    updateRequirementTileData(host, requirementTrackerTile, trackerJsonRoot)
                end
            end
        end
    end

    combatOverlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number")
    end

    combatOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        levelProgressContainer:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            combatOverlayCanvas.widgetName, 10)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            combatOverlayCanvas.widgetName, 10)
        local levelProgressContainerWidth = windowWidth / 5.0
        levelProgressContainer:setWidth(levelProgressContainerWidth)
        levelProgressContainer:setHeight(levelProgressContainerWidth * 0.5)

        levelProgressRowLayout:setParent(host, combatOverlayCanvas.widgetName, levelProgressContainer.widgetName)
        levelProgressRowLayout:fill(levelProgressContainer.widgetName)
        levelProgressRowLayout:setSpacing(35)
        levelProgressRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)
    end)

    combatOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if "LevelProgressChanged" == eventName then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["level_progress_status_type"] ~= nil then
                local lvlProgressStatusType = tonumber(parsedJson["level_progress_status_type"])
                if lvlProgressStatusType == LevelProgressStatusType.CURRENT_STAGE_CHANGED then
                    print("CURRENT_STAGE_CHANGED")
                    combatOverlay.onCurrentLevelProgressStageChanged()
                elseif lvlProgressStatusType == LevelProgressStatusType.REQUIREMENT_TRACKERS_STATUS_CHANGED then
                    print("REQUIREMENT_TRACKERS_STATUS_CHANGED")
                    combatOverlay.onRequirementTrackersStatusChanged()
                end
            end
        end
    end

    combatOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "WindowSizeChanged" == eventName and combatOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            local windowSize = {}
            if parsedJson["width"] ~= nil then
                windowSize.width = tonumber(parsedJson["width"])
            end
            if parsedJson["height"] ~= nil then
                windowSize.height = tonumber(parsedJson["height"])
            end
            combatOverlay.onWindowSizeChanged(windowSize.width, windowSize.height)
        end
    end

    return combatOverlay
end

return CombatOverlay
