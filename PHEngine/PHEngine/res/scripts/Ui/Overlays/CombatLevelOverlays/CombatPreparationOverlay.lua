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
    local panelWidth = (mainButtonSize * 2) + 70
    local gridWidth = smallButtonSize * 3 + 90
    local gridHeight = smallButtonSize * 2 + 60

    local backgroundRect = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(backgroundRect)

    local combatPreparationRowLayout = UiRowLayout:new(host, "CombatPreparationRowLayout")
    combatPreparationOverlay:addWidget(combatPreparationRowLayout)

    local createObjectButton = ImageButton:new(host, combatPreparationOverlay, "CreateObjectButton")
    combatPreparationOverlay:addCompoundWidget(createObjectButton)

    local removeObjectButton = ImageButton:new(host, combatPreparationOverlay, "RemoveObjectButton")
    combatPreparationOverlay:addCompoundWidget(removeObjectButton)

    local gridBackground = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(gridBackground)

    local createMenuDropDownGridLayout = UiGridLayout:new(host, "CreateMenuDropDownGridLayout")
    combatPreparationOverlay:addWidget(createMenuDropDownGridLayout)

    local removeDropDownPanel = UiItem:new(host, "RemoveDropDownPanel")
    combatPreparationOverlay:addWidget(removeDropDownPanel)

    local removeMenuDropDownRowLayout = UiRowLayout:new(host, "RemoveMenuDropDownRowLayout")
    combatPreparationOverlay:addWidget(removeMenuDropDownRowLayout)

    local discardRemoveTowerButton = ImageButton:new(host, combatPreparationOverlay, "DiscardRemoveTowerButton")
    combatPreparationOverlay:addCompoundWidget(discardRemoveTowerButton)

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

    local discardCreateTowerButton = ImageButton:new(host, combatPreparationOverlay, "DiscardCreateTowerButton")
    combatPreparationOverlay:addCompoundWidget(discardCreateTowerButton)

    local completeStageButton = ImageButton:new(host, combatPreparationOverlay, "CompleteStageButton")
    combatPreparationOverlay:addCompoundWidget(completeStageButton)

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

    discardCreateTowerButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "ghost_tower_visibility", visible = false}))
    end)

    discardCreateTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    removeObjectButton:subscribeOnMouseInputClickedCallback(function()
        hideCreatePanel()
        removeDropDownPanel:setIsVisible(true);
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

    discardRemoveTowerButton:subscribeOnMouseInputClickedCallback(function()
        hideRemovePanel()
        discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = false}))
    end)

    discardRemoveTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
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
        createObjectButton:setImageTextureSource("plus.png")

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

        createMenuDropDownGridLayout:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                               gridBackground.widgetName)
        createMenuDropDownGridLayout:fill(gridBackground.widgetName)
        createMenuDropDownGridLayout:setHorizontalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setVerticalSpacing(smallButtonSize * 0.5)
        createMenuDropDownGridLayout:setColumnsCount(3)
        createMenuDropDownGridLayout:setRowsCount(2)
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
        end

        discardCreateTowerButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                           createMenuDropDownGridLayout.widgetName)
        discardCreateTowerButton:setWidth(smallButtonSize)
        discardCreateTowerButton:setHeight(smallButtonSize)
        discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        discardCreateTowerButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        discardCreateTowerButton:setImageTextureSource("cancel.png")

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

        removeObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                     combatPreparationRowLayout.widgetName)
        removeObjectButton:setWidth(mainButtonSize)
        removeObjectButton:setHeight(mainButtonSize)
        removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        removeObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        removeObjectButton:setImageTextureSource("minus.png")

        removeDropDownPanel:setParent(host, combatPreparationOverlayCanvas.widgetName, backgroundRect.widgetName)
        removeDropDownPanel:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                      UiItemBase.UiAnchorType.HORIZONTAL_CENTER, backgroundRect.widgetName)
        removeDropDownPanel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                      backgroundRect.widgetName)
        removeDropDownPanel:setWidth(smallButtonSize * 2 + 40)
        removeDropDownPanel:setHeight(smallButtonSize)
        removeDropDownPanel:setVerticalCenterOffset(panelHeight)
        removeDropDownPanel:setIsVisible(false)

        removeMenuDropDownRowLayout:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                              removeDropDownPanel.widgetName)
        removeMenuDropDownRowLayout:fill(removeDropDownPanel.widgetName)
        removeMenuDropDownRowLayout:setSpacing(smallButtonSize * 0.5)
        removeMenuDropDownRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)

        discardRemoveTowerButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                           removeMenuDropDownRowLayout.widgetName)
        discardRemoveTowerButton:setWidth(smallButtonSize)
        discardRemoveTowerButton:setHeight(smallButtonSize)
        discardRemoveTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        discardRemoveTowerButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        discardRemoveTowerButton:setImageTextureSource("cancel.png")
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

    return combatPreparationOverlay
end

return CombatPreparationOverlay
