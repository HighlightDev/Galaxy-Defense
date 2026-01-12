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
local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")
local UiRowLayout = require("Ui/Core/uiRowLayout")
local ImageButton = require("Ui/Widgets/ImageButton")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local UiGridLayout = require("Ui/Core/uiGridLayout")
local UiImageButton = require("Ui/Widgets/ImageButton")

CombatPreparationOverlay = {buttonRadius = 6, localTimerManager = nil}

local GameModeType = {INIT = 0, COMBAT = 1, SPACE_STATION_PLACEMENT = 2}

local MissileType = {BOMB = 0, FREEZING_BOMB = 1, ELECTRO_RAY = 2, FREEZING_RAY = 3, BLACK_HOLE = 4}

local function valueToMissileType(value)
    for k, v in pairs(MissileType) do if v == value then return k end end
    return nil
end

local createTowerButtons = {}

function CombatPreparationOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local combatPreparationOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight,
                                                        "CombatPreparationOverlayCanvas")
    combatPreparationOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, combatPreparationOverlayCanvas.luaProxyId)
    end)
    local combatPreparationOverlay = UiOverlay:createOverlay(host, "CombatPreparationOverlay",
                                                             combatPreparationOverlayCanvas)

    combatPreparationOverlay.localTimerManager = combatPreparationOverlay:getTimerManager()

    local panelHeight = windowHeight * 0.15
    local mainButtonSize = panelHeight * 0.8
    local smallButtonSize = windowHeight * 0.07
    local panelWidth = (mainButtonSize) + 50
    local gridHeader = smallButtonSize * 0.5
    local gridWidth = smallButtonSize * 3 + (smallButtonSize * 0.5) * 3
    local gridHeight = smallButtonSize * 3 + (smallButtonSize * 0.5) * 3 + gridHeader

    local backgroundRect = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(backgroundRect)

    local combatPreparationRowLayout = UiRowLayout:new(host, "CombatPreparationRowLayout")
    combatPreparationOverlay:addWidget(combatPreparationRowLayout)

    local createObjectButton = ImageButton:new(host, combatPreparationOverlay, "CreateObjectButton")
    combatPreparationOverlay:addCompoundWidget(createObjectButton)

    local removeObjectButton = ImageButton:new(host, combatPreparationOverlay, "RemoveObjectButton")
    combatPreparationOverlay:addCompoundWidget(removeObjectButton)

    local barrierManagementButton = ImageButton:new(host, combatPreparationOverlay, "BarrierManagementButton")
    combatPreparationOverlay:addCompoundWidget(barrierManagementButton)

    local gridBackground = UiRectangle:new(host, "GridBackground")
    combatPreparationOverlay:addWidget(gridBackground)

    local gridBackgroundHeader = UiRectangle:new(host, "GridBackgroundHeader")
    combatPreparationOverlay:addWidget(gridBackgroundHeader)

    local createMenuDropDownGridLayout = UiGridLayout:new(host, "CreateMenuDropDownGridLayout")
    combatPreparationOverlay:addWidget(createMenuDropDownGridLayout)

    for i = MissileType.BOMB, MissileType.BLACK_HOLE do
        local button = ImageButton:new(host, combatPreparationOverlay, "CreateTowerButton" .. tostring(i))
        combatPreparationOverlay:addCompoundWidget(button)
        createTowerButtons[#createTowerButtons + 1] = button
        local missileType = valueToMissileType(i)

        button:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                button:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                button:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end)

        button:subscribeOnMouseInputClickedCallback(function()
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "CombatLevelEvents", json.encode(
                                                          {
                    action = "ghost_tower_visibility",
                    tower_type = tostring(missileType),
                    visible = true
                }))
        end)
    end

    local closeTowerCreatePanelButton = UiImageButton:new(host, combatPreparationOverlay, "closeTowerCreatePanelButton")
    combatPreparationOverlay:addCompoundWidget(closeTowerCreatePanelButton)

    local completeStageButton = ImageButton:new(host, combatPreparationOverlay, "CompleteStageButton")
    combatPreparationOverlay:addCompoundWidget(completeStageButton)

    local hideCreatePanel = function() gridBackground:setIsVisible(false) end

    createObjectButton:subscribeOnMouseInputClickedCallback(function()
        gridBackground:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = false}))
    end)

    createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            createObjectButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    closeTowerCreatePanelButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "ghost_tower_visibility", visible = false}))
    end)

    closeTowerCreatePanelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    removeObjectButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = true}))
    end)

    removeObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            removeObjectButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    barrierManagementButton:subscribeOnMouseInputClickedCallback(function() hideCreatePanel() end)

    barrierManagementButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            barrierManagementButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            barrierManagementButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local canCompletePreparationStage = false

    completeStageButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        EventsHelper:sendChangeGameModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       GameModeType.COMBAT)
    end)

    completeStageButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if canCompletePreparationStage then
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                completeStageButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                completeStageButton:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end
    end)

    combatPreparationOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("combatPreparationOverlay:OnAllWidgetLuaProxiesReady: name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                 combatPreparationOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 combatPreparationOverlayCanvas.widgetName)
        backgroundRect:setWidth(panelWidth)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                 combatPreparationOverlayCanvas.widgetName, 0)
        backgroundRect:setColorHexValue(Styles.Colors.panelColor)
        backgroundRect:setZOrder(1)
        backgroundRect:setHeight(panelHeight)
        backgroundRect:setBorderRadius(CombatPreparationOverlay.buttonRadius)

        combatPreparationRowLayout:setParent(host, combatPreparationOverlayCanvas.widgetName, backgroundRect.widgetName)
        combatPreparationRowLayout:fill(backgroundRect.widgetName)
        combatPreparationRowLayout:setSpacing(45)
        combatPreparationRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)

        createObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                     combatPreparationRowLayout.widgetName)
        createObjectButton:setWidth(mainButtonSize)
        createObjectButton:setHeight(mainButtonSize)
        createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        createObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        createObjectButton:setImageTextureSource("hammer.png")
        createObjectButton:setImageRotationDegrees(180)

        gridBackground:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                 combatPreparationOverlayCanvas.widgetName)
        gridBackground:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName,
                                 50)
        gridBackground:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                 backgroundRect.widgetName)
        gridBackground:setWidth(gridWidth)
        gridBackground:setHeight(gridHeight)
        gridBackground:setColorHexValue(Styles.Colors.panelColor)
        gridBackground:setBorderRadius(CombatPreparationOverlay.buttonRadius)
        gridBackground:setIsVisible(false)
        gridBackground:setZOrder(2)

        gridBackgroundHeader:setParent(host, combatPreparationOverlayCanvas.widgetName, gridBackground.widgetName)
        gridBackgroundHeader:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                       gridBackground.widgetName)
        gridBackgroundHeader:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                       UiItemBase.UiAnchorType.HORIZONTAL_CENTER, gridBackground.widgetName)
        gridBackgroundHeader:setWidth(gridWidth)
        gridBackgroundHeader:setHeight(gridHeader)
        gridBackgroundHeader:setColorHexValue(Styles.Colors.headerPanelColor)
        gridBackgroundHeader:setBorderRadius(CombatPreparationOverlay.buttonRadius)
        gridBackgroundHeader:setIsRoundBottom(false)
        gridBackgroundHeader:setZOrder(3)

        createMenuDropDownGridLayout:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                               gridBackgroundHeader.widgetName, 0)
        createMenuDropDownGridLayout:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:setZOrder(3)
        createMenuDropDownGridLayout:setHorizontalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setVerticalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setColumnsCount(3)
        createMenuDropDownGridLayout:setRowsCount(3)
        createMenuDropDownGridLayout:setAlignment(UiGridLayout.UiGridHorizontalAlignmentType.CENTER,
                                                  UiGridLayout.UiGridVerticalAlignmentType.CENTER)

        for i = 1, #createTowerButtons do
            createTowerButtons[i]:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                            createMenuDropDownGridLayout.widgetName)
            createTowerButtons[i]:setWidth(smallButtonSize)
            createTowerButtons[i]:setHeight(smallButtonSize)
            createTowerButtons[i]:setButtonColorHexValue(Styles.Colors.buttonColor)
            createTowerButtons[i]:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
            createTowerButtons[i]:setImageTextureSource("space_station_img.png")
            createTowerButtons[i]:setImageRotationDegrees(180)
            createTowerButtons[i]:setZOrder(3)
        end

        barrierManagementButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                          createMenuDropDownGridLayout.widgetName)
        barrierManagementButton:setWidth(smallButtonSize)
        barrierManagementButton:setHeight(smallButtonSize)
        barrierManagementButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        barrierManagementButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        barrierManagementButton:setImageTextureSource("warning.png")

        removeObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                     createMenuDropDownGridLayout.widgetName)
        removeObjectButton:setWidth(smallButtonSize)
        removeObjectButton:setHeight(smallButtonSize)
        removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        removeObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        removeObjectButton:setImageTextureSource("trash.png")
        removeObjectButton:setImageRotationDegrees(180)

        closeTowerCreatePanelButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                              gridBackgroundHeader.widgetName)
        closeTowerCreatePanelButton:setWidth(smallButtonSize * 0.4)
        closeTowerCreatePanelButton:setHeight(smallButtonSize * 0.4)
        closeTowerCreatePanelButton:setUseImageCustomColor(true)
        closeTowerCreatePanelButton:setImageColorHexValue(0x000000)
        closeTowerCreatePanelButton:setImageTextureSource("cancel.png")
        closeTowerCreatePanelButton:setIsBackgroundVisible(false)
        closeTowerCreatePanelButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                              gridBackgroundHeader.widgetName, 0)
        closeTowerCreatePanelButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                              gridBackgroundHeader.widgetName, 0)
        closeTowerCreatePanelButton:setZOrder(4)

        completeStageButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                      combatPreparationOverlayCanvas.widgetName)
        completeStageButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                      combatPreparationOverlayCanvas.widgetName, 20)
        completeStageButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                      combatPreparationOverlayCanvas.widgetName, 20)
        completeStageButton:setWidth(mainButtonSize)
        completeStageButton:setHeight(mainButtonSize)
        completeStageButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        completeStageButton:setImageTextureSource("flag-banner-fold.png")
        completeStageButton:setImageRotationDegrees(180)
        completeStageButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    end)

    combatPreparationOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if "PlayerStatusChanged" == eventName then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["towers_count"] ~= nil then
                local newTowersCount = tonumber(parsedJson["towers_count"])
                if newTowersCount > 0 then
                    canCompletePreparationStage = true
                else
                    canCompletePreparationStage = false
                end
            end
        end
    end
    combatPreparationOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    combatPreparationOverlay.derivedUpdateCallback = function()
        if combatPreparationOverlay.allWidgetLuaProxiesReady then
            completeStageButton:setIsButtonActive(canCompletePreparationStage)
        end
    end

    combatPreparationOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
        if "CombatLevelEvents" == eventName and combatPreparationOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["action"] ~= nil then
                local action = tostring(parsedJson["action"])
                if action == "switch_mode" then
                    local mode = tostring(parsedJson["mode"])
                    if mode == "IDLE" then
                        hideCreatePanel()
                        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                                  "CombatLevelEvents", json.encode(
                                                                      {
                                action = "ghost_tower_visibility",
                                visible = false
                            }))
                    end
                end
            end
        end
    end

    return combatPreparationOverlay
end

return CombatPreparationOverlay
