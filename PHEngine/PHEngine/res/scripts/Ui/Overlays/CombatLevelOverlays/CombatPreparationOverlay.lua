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
local ImageButton = require("Ui/Widgets/ImageButton")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")

CombatPreparationOverlay = {
    buttonRadius = 6,
    localTimerManager = nil
}

GameModeType = {
    INIT = 0,
    COMBAT = 1,
    SPACE_STATION_PLACEMENT = 2
}

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
    local smallButtonSize = mainButtonSize * 0.7
    local panelWidth = (mainButtonSize * 2) + 70

    local backgroundRect = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(backgroundRect)

    local combatPreparationRowLayout = UiRowLayout:new(host, "CombatPreparationRowLayout")
    combatPreparationOverlay:addWidget(combatPreparationRowLayout)

    local createObjectButton = ImageButton:new(host, combatPreparationOverlay, "CreateObjectButton")
    combatPreparationOverlay:addCompoundWidget(createObjectButton)

    local combatPreparationRowLayout = UiRowLayout:new(host, "CombatPreparationRowLayout")
    combatPreparationOverlay:addWidget(combatPreparationRowLayout)

    local removeObjectButton = ImageButton:new(host, combatPreparationOverlay, "RemoveObjectButton")
    combatPreparationOverlay:addCompoundWidget(removeObjectButton)

    local createDropDownPanel = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(createDropDownPanel)

    local createMenuDropDownRowLayout = UiRowLayout:new(host, "CreateMenuDropDownRowLayout")
    combatPreparationOverlay:addWidget(createMenuDropDownRowLayout)

    local createTowerButton = ImageButton:new(host, combatPreparationOverlay, "CreateTowerButton")
    combatPreparationOverlay:addCompoundWidget(createTowerButton)

    local discardCreateTowerButton = ImageButton:new(host, combatPreparationOverlay, "DiscardCreateTowerButton")
    combatPreparationOverlay:addCompoundWidget(discardCreateTowerButton)

    local completeStageButton = ImageButton:new(host, combatPreparationOverlay, "CompleteStageButton")
    combatPreparationOverlay:addCompoundWidget(completeStageButton)

    local timerRunning = false
    local timeoutAction = function()
        timerRunning = false
        discardCreateTowerButton:setIsVisible(false)
        createDropDownPanel:setIsVisible(false)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
            "CombatLevelEvents", json.encode({
                action = "tower_grid_visibility",
                visible = false
            }))
    end

    local hidePopupTimerId = combatPreparationOverlay.localTimerManager:createTimer(timeoutAction, false, 20.0)

    createObjectButton:subscribeOnMouseInputClickedCallback(function()
        createDropDownPanel:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
            "CombatLevelEvents", json.encode({
                action = "tower_grid_visibility",
                visible = true
            }))
        combatPreparationOverlay.localTimerManager:restartTimer(hidePopupTimerId)
        timerRunning = true
    end)


    createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            createObjectButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    createTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            createTowerButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            if timerRunning then
                combatPreparationOverlay.localTimerManager:stopTimer(hidePopupTimerId)
            end
        else
            createTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
            if timerRunning then
                combatPreparationOverlay.localTimerManager:restartTimer(hidePopupTimerId)
            end
        end
    end)

    createTowerButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
            "CombatLevelEvents", json.encode({
                action = "ghost_tower_visibility",
                visible = true
            }))
        combatPreparationOverlay.localTimerManager:stopTimer(hidePopupTimerId)
        timerRunning = false
        discardCreateTowerButton:setIsVisible(true)
    end)

    discardCreateTowerButton:subscribeOnMouseInputClickedCallback(function()
        timeoutAction()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
            "CombatLevelEvents", json.encode({
                action = "ghost_tower_visibility",
                visible = false
            }))
    end)

    discardCreateTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    removeObjectButton:subscribeOnMouseInputClickedCallback(function()
        timeoutAction()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
            "CombatLevelEvents", json.encode({
                action = "remove_tower_marker_visibility",
                visible = true
            }))
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
        timeoutAction()
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
        print("combatPreparationOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

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

        removeObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            combatPreparationRowLayout.widgetName)
        removeObjectButton:setWidth(mainButtonSize)
        removeObjectButton:setHeight(mainButtonSize)
        removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        removeObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        removeObjectButton:setImageTextureSource("minus.png")

        createDropDownPanel:setParent(host, combatPreparationOverlayCanvas.widgetName,
            backgroundRect.widgetName)
        createDropDownPanel:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            UiItemBase.UiAnchorType.HORIZONTAL_CENTER, backgroundRect.widgetName)
        createDropDownPanel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
            UiItemBase.UiAnchorType.VERTICAL_CENTER, backgroundRect.widgetName)
        createDropDownPanel:setWidth(smallButtonSize * 2 + 40)
        createDropDownPanel:setHeight(smallButtonSize)
        createDropDownPanel:setIsVisible(false)
        createDropDownPanel:setColorHexValue(Styles.Colors.panelColor)
        createDropDownPanel:setVerticalCenterOffset(panelHeight)
        createDropDownPanel:setBorderRadius(CombatPreparationOverlay.buttonRadius)
        createDropDownPanel:setOpacity(0.0)

        createMenuDropDownRowLayout:setParent(host, combatPreparationOverlayCanvas.widgetName,
            createDropDownPanel.widgetName)
        createMenuDropDownRowLayout:fill(createDropDownPanel.widgetName)
        createMenuDropDownRowLayout:setSpacing(smallButtonSize * 0.5)
        createMenuDropDownRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.LEFT)

        createTowerButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            createMenuDropDownRowLayout.widgetName)
        createTowerButton:setWidth(smallButtonSize)
        createTowerButton:setHeight(smallButtonSize)
        createTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        createTowerButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        createTowerButton:setImageTextureSource("space_station_img.png")
        createTowerButton:setImageRotationDegrees(180)

        discardCreateTowerButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            createMenuDropDownRowLayout.widgetName)
        discardCreateTowerButton:setWidth(smallButtonSize)
        discardCreateTowerButton:setHeight(smallButtonSize)
        discardCreateTowerButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        discardCreateTowerButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        discardCreateTowerButton:setImageTextureSource("cancel.png")
        discardCreateTowerButton:setIsVisible(false)

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

    return combatPreparationOverlay
end

return CombatPreparationOverlay
