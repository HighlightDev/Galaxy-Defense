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
local UiRowLayout = require("Ui/Core/uiRowLayout")
local ImageAndLabelTile = require("Ui/Widgets/ImageAndLabelTile")
local Styles = require("Ui/Common/styles")
local UiTextBlock = require("Ui/Core/uiTextBlock")
local UiGridLayout = require("Ui/Core/uiGridLayout")

local LevelProgressStatusType = {
    NONE = 0,
    CURRENT_STAGE_CHANGED = 1,
    REQUIREMENT_TRACKERS_STATUS_CHANGED = 2
}

local MissileType = {
    BOMB = 0,
    FREEZING_BOMB = 1,
    ELECTRO_RAY = 2,
    FREEZING_RAY = 3,
    BLACK_HOLE = 4
}

local function valueToMissileType(value)
    for k, v in pairs(MissileType) do if v == value then return k end end
    return nil
end

local CombatOverlay = {
    levelProgressContainer = nil,
    levelProgressRowLayout = nil
}

local RequirementTrackers = {}
local RequirementTrackersIdle = {}

local RequirementTrackerHint = nil

local createTowerButtons = {}

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
        local leftSpaceshipsCount =
            trackerJson["left_to_destroy_spaceships_count"]
        if tonumber(leftSpaceshipsCount) > 0 then
            levelProgressTile:setLabelText(leftSpaceshipsCount)
            levelProgressTile:setTextureSource("skull.png")
        else
            showTileRequirementAchived(levelProgressTile)
        end
    elseif name == "MissedSpaceshipsTracker" then
        local doNotMissCount = tonumber(
                                   trackerJson["not_to_miss_spaceships_count"])
        local missedCount = tonumber(trackerJson["missed_spaceships_count"])
        local stillCanMiss = math.max(doNotMissCount - missedCount, 0)
        if stillCanMiss > 0 then
            levelProgressTile:setLabelText(tostring(stillCanMiss))
            levelProgressTile:setTextureSource("warning.png")
        else
            showTileRequirementFailed(levelProgressTile)
            UiOverlayManager:openOverlay(host, "LevelFailedOverlay")
            EventsHelper:sendPauseGameThreadEvent(host,
                                                  EventsHelper.enqueueJobPolicy
                                                      .IF_DUPLICATE_NO_PUSH,
                                                  true)
        end
    else
        assert(false, "Not supported requirement: " .. name)
    end
    local hintText = tostring(trackerJson["hint"])
    levelProgressTile.hintText = hintText
end

local function fillRequirementTilesPool(host, combatOverlay, count)
    for i = 1, count, 1 do
        local levelProgressTile = ImageAndLabelTile:new(host, combatOverlay)
        combatOverlay:addCompoundWidget(levelProgressTile)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] =
            levelProgressTile

        levelProgressTile:subscribeOnLuaProxiesReady(function(host)
            levelProgressTile:setParent(host,
                                        combatOverlay:getOverlayCanvas().widgetName,
                                        combatOverlay.levelProgressRowLayout
                                            .widgetName)
            local tileSize = 100 -- temporary for now
            levelProgressTile:setWidth(tileSize)
            levelProgressTile:setHeight(tileSize)
            levelProgressTile:setBackgroundTileOpacity(1.0)
            levelProgressTile:setIsVisible(false)
            levelProgressTile:setBackgroundTileColorHexValue(Styles.Colors
                                                                 .panelColor)
            levelProgressTile:subscribeOnMouseInputCursorHoverStateChangedCallback(
                function(newState)
                    if newState ==
                        UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                        RequirementTrackerHint:setIsVisible(true)
                        RequirementTrackerHint:setAttachTargetUiItemName(
                            levelProgressTile.widgetName)
                        RequirementTrackerHint:setText(
                            levelProgressTile.hintText)
                    else
                        RequirementTrackerHint:setIsVisible(false)
                        RequirementTrackerHint:setAttachTargetUiItemName("")
                    end
                end)
        end)
    end
end

local function updateRequirementTiles(host, combatOverlay, requirementTrackers)
    -- move active requirements tiles to idle state
    for _, activeTrackerTile in pairs(RequirementTrackers) do
        activeTrackerTile:setIsVisible(false)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] =
            activeTrackerTile
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

    local combatOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth,
                                             windowHeight)
    combatOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, combatOverlayCanvas.luaProxyId)
    end)
    local combatOverlay = UiOverlay:createOverlay(host, "CombatOverlay",
                                                  combatOverlayCanvas)

    local levelProgressContainer = UiItem:new(host, "LvlProgressContainer")
    combatOverlay:addWidget(levelProgressContainer)
    combatOverlay.levelProgressContainer = levelProgressContainer

    local levelProgressRowLayout = UiRowLayout:new(host, "LvlProgressRow")
    combatOverlay:addWidget(levelProgressRowLayout)
    combatOverlay.levelProgressRowLayout = levelProgressRowLayout

    RequirementTrackerHint = UiTextBlock:new(host, "Lora-VariableFont_wght",
                                             "TestTextBlock")
    combatOverlay:addWidget(RequirementTrackerHint)

    fillRequirementTilesPool(host, combatOverlay, 10)

    local panelHeight = windowHeight * 0.15
    local mainButtonSize = panelHeight * 0.8
    local smallButtonSize = windowHeight * 0.07
    local panelWidth = (mainButtonSize * 2) + 70
    local gridWidth = smallButtonSize * 3 + 90
    local gridHeight = smallButtonSize * 2 + 60

    local backgroundRect = UiRectangle:new(host)
    combatOverlay:addWidget(backgroundRect)

    local combatPreparationRowLayout = UiRowLayout:new(host,
                                                       "CombatPreparationRowLayout")
    combatOverlay:addWidget(combatPreparationRowLayout)

    local createObjectButton = ImageButton:new(host, combatOverlay,
                                               "CreateObjectButton")
    combatOverlay:addCompoundWidget(createObjectButton)

    local combatPreparationRowLayout = UiRowLayout:new(host,
                                                       "CombatPreparationRowLayout")
    combatOverlay:addWidget(combatPreparationRowLayout)

    local removeObjectButton = ImageButton:new(host, combatOverlay,
                                               "RemoveObjectButton")
    combatOverlay:addCompoundWidget(removeObjectButton)

    local gridBackground = UiRectangle:new(host, "GridBackground")
    combatOverlay:addWidget(gridBackground)

    local createMenuDropDownGridLayout =
        UiGridLayout:new(host, "CreateMenuDropDownGridLayout")
    combatOverlay:addWidget(createMenuDropDownGridLayout)

    local removeDropDownPanel = UiItem:new(host, "RemoveDropDownPanel")
    combatOverlay:addWidget(removeDropDownPanel)

    local removeMenuDropDownRowLayout = UiRowLayout:new(host,
                                                        "RemoveMenuDropDownRowLayout")
    combatOverlay:addWidget(removeMenuDropDownRowLayout)

    local discardRemoveTowerButton = ImageButton:new(host, combatOverlay,
                                                     "DiscardRemoveTowerButton")
    combatOverlay:addCompoundWidget(discardRemoveTowerButton)

    for i = MissileType.BOMB, MissileType.BLACK_HOLE do
        local button = ImageButton:new(host, combatOverlay,
                                       "CreateTowerButton" .. tostring(i))
        combatOverlay:addCompoundWidget(button)
        createTowerButtons[#createTowerButtons + 1] = button
        local missileType = valueToMissileType(i)

        button:subscribeOnMouseInputCursorHoverStateChangedCallback(function(
            newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                button:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                button:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end)

        button:subscribeOnMouseInputClickedCallback(function()
            EventsHelper:sendBroadcastGameThreadEvent(host,
                                                      EventsHelper.enqueueJobPolicy
                                                          .PUSH_ANYWAY,
                                                      "CombatLevelEvents",
                                                      json.encode({
                action = "ghost_tower_visibility",
                tower_type = tostring(missileType),
                visible = true
            }))
        end)
    end

    local discardCreateTowerButton = ImageButton:new(host, combatOverlay,
                                                     "DiscardCreateTowerButton")
    combatOverlay:addCompoundWidget(discardCreateTowerButton)

    local hideCreatePanel = function()
        discardCreateTowerButton:setIsVisible(false)
        gridBackground:setIsVisible(false)
    end

    local hideRemovePanel = function()
        discardRemoveTowerButton:setIsVisible(false)
        removeDropDownPanel:setIsVisible(false)
    end

    createObjectButton:subscribeOnMouseInputClickedCallback(function()
        hideRemovePanel()
        gridBackground:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host,
                                                  EventsHelper.enqueueJobPolicy
                                                      .PUSH_ANYWAY,
                                                  "CombatLevelEvents",
                                                  json.encode(
                                                      {
                action = "remove_tower_marker_visibility",
                visible = false
            }))
    end)

    createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(
        function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                createObjectButton:setButtonColorHexValue(Styles.Colors
                                                              .hoveredButtonColor)
            else
                createObjectButton:setButtonColorHexValue(
                    Styles.Colors.buttonColor)
            end
        end)

    discardCreateTowerButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        discardCreateTowerButton:setButtonColorHexValue(Styles.Colors
                                                            .buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host,
                                                  EventsHelper.enqueueJobPolicy
                                                      .PUSH_ANYWAY,
                                                  "CombatLevelEvents",
                                                  json.encode(
                                                      {
                action = "ghost_tower_visibility",
                visible = false
            }))
    end)

    discardCreateTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(
        function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                discardCreateTowerButton:setButtonColorHexValue(Styles.Colors
                                                                    .hoveredButtonColor)
            else
                discardCreateTowerButton:setButtonColorHexValue(Styles.Colors
                                                                    .buttonColor)
            end
        end)

    removeObjectButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        removeDropDownPanel:setIsVisible(true);
        EventsHelper:sendBroadcastGameThreadEvent(host,
                                                  EventsHelper.enqueueJobPolicy
                                                      .PUSH_ANYWAY,
                                                  "CombatLevelEvents",
                                                  json.encode(
                                                      {
                action = "remove_tower_marker_visibility",
                visible = true
            }))
    end)

    removeObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(
        function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                removeObjectButton:setButtonColorHexValue(Styles.Colors
                                                              .hoveredButtonColor)
            else
                removeObjectButton:setButtonColorHexValue(
                    Styles.Colors.buttonColor)
            end
        end)

    discardRemoveTowerButton:subscribeOnMouseInputClickedCallback(function()
        hideRemovePanel()
        discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors
                                                            .buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host,
                                                  EventsHelper.enqueueJobPolicy
                                                      .PUSH_ANYWAY,
                                                  "CombatLevelEvents",
                                                  json.encode(
                                                      {
                action = "remove_tower_marker_visibility",
                visible = false
            }))
    end)

    discardRemoveTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(
        function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors
                                                                    .hoveredButtonColor)
            else
                discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors
                                                                    .buttonColor)
            end
        end)

    combatOverlay.onCurrentLevelProgressStageChanged = function()
        local currentProgressRequirementsCount =
            _GetCurrentProgressRequirementsCount(host)
        print(
            "CombatOverlay::onCurrentLevelProgressStageChanged : req count: " ..
                tostring(currentProgressRequirementsCount))

        if currentProgressRequirementsCount > 0 then
            local requirementTrackersJson =
                _GetCurrentProgressStageRequirementTrackers(host)
            if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
                local parsedJson = json.decode(requirementTrackersJson)
                updateRequirementTiles(host, combatOverlay, parsedJson)
            end
        elseif #RequirementTrackers > 0 then
            showAllRequirementsAchived()
        end
    end

    combatOverlay.onRequirementTrackersStatusChanged = function()
        local requirementTrackersJson =
            _GetCurrentProgressStageRequirementTrackers(host)
        if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
            local parsedJson = json.decode(requirementTrackersJson)
            if parsedJson ~= nil and parsedJson ~= "" then
                assert(#parsedJson == #RequirementTrackers)
                for index, trackerJsonRoot in pairs(parsedJson) do
                    local requirementTrackerTile = RequirementTrackers[index]
                    assert(requirementTrackerTile ~= nil)
                    updateRequirementTileData(host, requirementTrackerTile,
                                              trackerJsonRoot)
                end
            end
        end
    end

    combatOverlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and
                   type(height) == "number")
    end

    combatOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        levelProgressContainer:setParent(host, combatOverlayCanvas.widgetName,
                                         combatOverlayCanvas.widgetName)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT,
                                         UiItemBase.UiAnchorType.RIGHT,
                                         combatOverlayCanvas.widgetName, 10)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.TOP,
                                         UiItemBase.UiAnchorType.TOP,
                                         combatOverlayCanvas.widgetName, 10)
        local levelProgressContainerWidth = windowWidth / 5.0
        levelProgressContainer:setWidth(levelProgressContainerWidth)
        levelProgressContainer:setHeight(levelProgressContainerWidth * 0.5)

        levelProgressRowLayout:setParent(host, combatOverlayCanvas.widgetName,
                                         levelProgressContainer.widgetName)
        levelProgressRowLayout:fill(levelProgressContainer.widgetName)
        levelProgressRowLayout:setSpacing(35)
        levelProgressRowLayout:setAlignment(
            UiRowLayout.UiRowAlignmentType.CENTER)

        RequirementTrackerHint:setParent(host, combatOverlayCanvas.widgetName,
                                         levelProgressContainer.widgetName)
        RequirementTrackerHint:setAnchor(UiItemBase.UiAnchorType.RIGHT,
                                         UiItemBase.UiAnchorType.RIGHT,
                                         levelProgressRowLayout.widgetName, 20)
        RequirementTrackerHint:setWidth(levelProgressContainerWidth)
        RequirementTrackerHint:setHeight(levelProgressContainerWidth)
        RequirementTrackerHint:setFontSize(15)
        RequirementTrackerHint:setRectangleOpacity(1.0)
        RequirementTrackerHint:setRectangleColor(0.6, 0.6, 0.6)
        RequirementTrackerHint:setTextColorHexValue(0x000000)
        RequirementTrackerHint:setTextHorizontalAlignment(
            UiLabel.TextHorizontalAlignmentType.CENTER)
        RequirementTrackerHint:setTextVerticalAlignment(
            UiLabel.TextVerticalAlignmentType.CENTER)
        RequirementTrackerHint:setRectangleRadius(6)
        RequirementTrackerHint:setBorderColorHexValue(0xFFFFFF)
        RequirementTrackerHint:setBorderRadius(0)
        RequirementTrackerHint:setBorderOpacity(1.0)
        RequirementTrackerHint:setIsVisible(false)
        RequirementTrackerHint:setBorderThickness(40)

        backgroundRect:setParent(host, combatOverlayCanvas.widgetName,
                                 combatOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 combatOverlayCanvas.widgetName)
        backgroundRect:setWidth(panelWidth)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM,
                                 UiItemBase.UiAnchorType.BOTTOM,
                                 combatOverlayCanvas.widgetName, 0)
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
        backgroundRect:setZOrder(1)
        backgroundRect:setHeight(panelHeight)
        backgroundRect:setBorderRadius(CombatPreparationOverlay.buttonRadius)

        combatPreparationRowLayout:setParent(host,
                                             combatOverlayCanvas.widgetName,
                                             backgroundRect.widgetName)
        combatPreparationRowLayout:fill(backgroundRect.widgetName)
        combatPreparationRowLayout:setSpacing(45)
        combatPreparationRowLayout:setAlignment(
            UiRowLayout.UiRowAlignmentType.CENTER)

        createObjectButton:setParent(host, combatOverlayCanvas.widgetName,
                                     combatPreparationRowLayout.widgetName)
        createObjectButton:setWidth(mainButtonSize)
        createObjectButton:setHeight(mainButtonSize)
        createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        createObjectButton:setButtonBorderRadius(
            CombatPreparationOverlay.buttonRadius)
        createObjectButton:setImageTextureSource("plus.png")

        gridBackground:setParent(host, combatOverlayCanvas.widgetName,
                                 combatOverlayCanvas.widgetName)
        gridBackground:setAnchor(UiItemBase.UiAnchorType.BOTTOM,
                                 UiItemBase.UiAnchorType.TOP,
                                 backgroundRect.widgetName, 50)
        gridBackground:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 backgroundRect.widgetName)
        gridBackground:setWidth(gridWidth)
        gridBackground:setHeight(gridHeight)
        gridBackground:setColorHexValue(Styles.Colors.panelColor)
        gridBackground:setBorderRadius(CombatPreparationOverlay.buttonRadius)
        gridBackground:setIsVisible(false)

        createMenuDropDownGridLayout:setParent(host,
                                               combatOverlayCanvas.widgetName,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:fill(gridBackground.widgetName)
        createMenuDropDownGridLayout:setHorizontalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setVerticalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setColumnsCount(3)
        createMenuDropDownGridLayout:setRowsCount(2)
        createMenuDropDownGridLayout:setAlignment(
            UiGridLayout.UiGridHorizontalAlignmentType.CENTER,
            UiGridLayout.UiGridVerticalAlignmentType.CENTER)

        for i = 1, #createTowerButtons do
            createTowerButtons[i]:setParent(host,
                                            combatOverlayCanvas.widgetName,
                                            createMenuDropDownGridLayout.widgetName)
            createTowerButtons[i]:setWidth(smallButtonSize)
            createTowerButtons[i]:setHeight(smallButtonSize)
            createTowerButtons[i]:setButtonColorHexValue(Styles.Colors
                                                             .buttonColor)
            createTowerButtons[i]:setButtonBorderRadius(
                CombatPreparationOverlay.buttonRadius)
            createTowerButtons[i]:setImageTextureSource("space_station_img.png")
            createTowerButtons[i]:setImageRotationDegrees(180)
        end

        discardCreateTowerButton:setParent(host, combatOverlayCanvas.widgetName,
                                           createMenuDropDownGridLayout.widgetName)
        discardCreateTowerButton:setWidth(smallButtonSize)
        discardCreateTowerButton:setHeight(smallButtonSize)
        discardCreateTowerButton:setButtonColorHexValue(Styles.Colors
                                                            .buttonColor)
        discardCreateTowerButton:setButtonBorderRadius(
            CombatPreparationOverlay.buttonRadius)
        discardCreateTowerButton:setImageTextureSource("cancel.png")

        removeObjectButton:setParent(host, combatOverlayCanvas.widgetName,
                                     combatPreparationRowLayout.widgetName)
        removeObjectButton:setWidth(mainButtonSize)
        removeObjectButton:setHeight(mainButtonSize)
        removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        removeObjectButton:setButtonBorderRadius(
            CombatPreparationOverlay.buttonRadius)
        removeObjectButton:setImageTextureSource("minus.png")

        removeDropDownPanel:setParent(host, combatOverlayCanvas.widgetName,
                                      backgroundRect.widgetName)
        removeDropDownPanel:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                      UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                      backgroundRect.widgetName)
        removeDropDownPanel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                      UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                      backgroundRect.widgetName)
        removeDropDownPanel:setWidth(smallButtonSize * 2 + 40)
        removeDropDownPanel:setHeight(smallButtonSize)
        removeDropDownPanel:setVerticalCenterOffset(panelHeight)
        removeDropDownPanel:setIsVisible(false)

        removeMenuDropDownRowLayout:setParent(host,
                                              combatOverlayCanvas.widgetName,
                                              removeDropDownPanel.widgetName)
        removeMenuDropDownRowLayout:fill(removeDropDownPanel.widgetName)
        removeMenuDropDownRowLayout:setSpacing(smallButtonSize * 0.5)
        removeMenuDropDownRowLayout:setAlignment(
            UiRowLayout.UiRowAlignmentType.CENTER)

        discardRemoveTowerButton:setParent(host, combatOverlayCanvas.widgetName,
                                           removeMenuDropDownRowLayout.widgetName)
        discardRemoveTowerButton:setWidth(smallButtonSize)
        discardRemoveTowerButton:setHeight(smallButtonSize)
        discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors
                                                            .buttonColor)
        discardRemoveTowerButton:setButtonBorderRadius(
            CombatPreparationOverlay.buttonRadius)
        discardRemoveTowerButton:setImageTextureSource("cancel.png")
    end)

    combatOverlay.onGameEventTriggered =
        function(eventName, jsonArgs)
            if "LevelProgressChanged" == eventName then
                assert(jsonArgs ~= nil and type(jsonArgs) == "string")
                local parsedJson = json.decode(jsonArgs)
                if parsedJson["level_progress_status_type"] ~= nil then
                    local lvlProgressStatusType = tonumber(
                                                      parsedJson["level_progress_status_type"])
                    if lvlProgressStatusType ==
                        LevelProgressStatusType.CURRENT_STAGE_CHANGED then
                        combatOverlay.onCurrentLevelProgressStageChanged()
                    elseif lvlProgressStatusType ==
                        LevelProgressStatusType.REQUIREMENT_TRACKERS_STATUS_CHANGED then
                        combatOverlay.onRequirementTrackersStatusChanged()
                    end
                end
            end
        end

    combatOverlay.onEngineEventTriggered =
        function(eventName, jsonArgs)
            if "WindowSizeChanged" == eventName and
                combatOverlay.allWidgetLuaProxiesReady == true then
                assert(jsonArgs ~= nil and type(jsonArgs) == "string")
                local parsedJson = json.decode(jsonArgs)
                local windowSize = {}
                if parsedJson["width"] ~= nil then
                    windowSize.width = tonumber(parsedJson["width"])
                end
                if parsedJson["height"] ~= nil then
                    windowSize.height = tonumber(parsedJson["height"])
                end
                combatOverlay.onWindowSizeChanged(windowSize.width,
                                                  windowSize.height)
            end
        end

    return combatOverlay
end

return CombatOverlay
