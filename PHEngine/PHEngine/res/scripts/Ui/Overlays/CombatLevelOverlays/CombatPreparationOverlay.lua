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

CombatPreparationOverlay = {
    buttonColor = 0x5e0d9a,
    hoveredButtonColor = 0x850edc,
    buttonRadius = 6,
    localTimerManager = nil
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
    local buttonSize = panelHeight * 0.8
    local panelWidth = (buttonSize * 2) + 70
    local buttonMarginHeight = (panelHeight - buttonSize) * 0.5

    local backgroundRect = UiRectangle:new(host)
    combatPreparationOverlay:addWidget(backgroundRect)

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

    local createMenuDropDownRowLayout = UiRowLayout:new(host, "CreateMenuDropDownRowLayout")
    combatPreparationOverlay:addWidget(createMenuDropDownRowLayout)

    local createTowerButton = ImageButton:new(host, combatPreparationOverlay, "CreateTowerButton")
    combatPreparationOverlay:addCompoundWidget(createTowerButton)

    local hidePopupTimerId = combatPreparationOverlay.localTimerManager:createTimer(function()
        createMenuDropDownRowLayout:setIsVisible(false)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "CombatLevelEvents", json.encode({
                action = "tower_grid_visibility",
                visible = false
            }))
    end, false, 20.0)

    createObjectButton:subscribeOnMouseInputClickedCallback(function()
        createMenuDropDownRowLayout:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "CombatLevelEvents", json.encode({
                action = "tower_grid_visibility",
                visible = true
            }))
        combatPreparationOverlay.localTimerManager:restartTimer(hidePopupTimerId)
    end)

    createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            createObjectButton:setButtonColorHexValue(CombatPreparationOverlay.hoveredButtonColor)
        else
            createObjectButton:setButtonColorHexValue(CombatPreparationOverlay.buttonColor)
        end
    end)

    createTowerButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            combatPreparationOverlay.localTimerManager:stopTimer(hidePopupTimerId)
        else
            combatPreparationOverlay.localTimerManager:restartTimer(hidePopupTimerId)
        end
    end)

    removeObjectButton:subscribeOnMouseInputClickedCallback(function()
    end)

    removeObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            removeObjectButton:setButtonColorHexValue(CombatPreparationOverlay.hoveredButtonColor)
        else
            removeObjectButton:setButtonColorHexValue(CombatPreparationOverlay.buttonColor)
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
        backgroundRect:setColorHexValue(0x6C5B7B)
        backgroundRect:setZOrder(1)
        backgroundRect:setHeight(panelHeight)
        backgroundRect:setBorderRadius(CombatPreparationOverlay.buttonRadius)

        combatPreparationRowLayout:setParent(host, combatPreparationOverlayCanvas.widgetName, backgroundRect.widgetName)
        combatPreparationRowLayout:fill(backgroundRect.widgetName)
        combatPreparationRowLayout:setSpacing(45)
        combatPreparationRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)

        createObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            combatPreparationRowLayout.widgetName)
        createObjectButton:setWidth(buttonSize)
        createObjectButton:setHeight(buttonSize)
        createObjectButton:setButtonColorHexValue(CombatPreparationOverlay.buttonColor)
        createObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        createObjectButton:setImageTextureSource("plus.png")

        removeObjectButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            combatPreparationRowLayout.widgetName)
        removeObjectButton:setWidth(buttonSize)
        removeObjectButton:setHeight(buttonSize)
        removeObjectButton:setButtonColorHexValue(CombatPreparationOverlay.buttonColor)
        removeObjectButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        removeObjectButton:setImageTextureSource("minus.png")

        createMenuDropDownRowLayout:setParent(host, combatPreparationOverlayCanvas.widgetName,
            backgroundRect.widgetName)
        createMenuDropDownRowLayout:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            UiItemBase.UiAnchorType.HORIZONTAL_CENTER, backgroundRect.widgetName)
        createMenuDropDownRowLayout:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
            UiItemBase.UiAnchorType.VERTICAL_CENTER, backgroundRect.widgetName)
        createMenuDropDownRowLayout:setVerticalCenterOffset(panelHeight - buttonMarginHeight)
        createMenuDropDownRowLayout:setSpacing(45)
        createMenuDropDownRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)
        createMenuDropDownRowLayout:setWidth(buttonSize + 10)
        createMenuDropDownRowLayout:setHeight(buttonSize + 10)
        createMenuDropDownRowLayout:setIsVisible(false)

        createTowerButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
            createMenuDropDownRowLayout.widgetName)
        createTowerButton:setWidth(buttonSize)
        createTowerButton:setHeight(buttonSize)
        createTowerButton:setButtonColorHexValue(CombatPreparationOverlay.buttonColor)
        createTowerButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        createTowerButton:setImageTextureSource("space_station_img.png")
        createTowerButton:setImageRotationDegrees(180)
    end)

    combatPreparationOverlay.onGameEventTriggered = function(eventName, jsonArgs) end
    combatPreparationOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    return combatPreparationOverlay
end

return CombatPreparationOverlay
