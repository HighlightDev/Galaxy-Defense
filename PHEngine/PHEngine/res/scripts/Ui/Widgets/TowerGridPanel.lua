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
local UiRectangle = require("Ui/Core/uiRectangle")
local UiRowLayout = require("Ui/Core/uiRowLayout")
local UiGridLayout = require("Ui/Core/uiGridLayout")
local ImageButton = require("Ui/Widgets/ImageButton")
local UiImageButton = require("Ui/Widgets/ImageButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local MissileTypes = require("Ui/Common/missileTypes")

local MissileType = MissileTypes.MissileType

TowerGridPanel = {DEFAULT_BUTTON_RADIUS = 6}

function TowerGridPanel:new(host, overlay, config)
    config = config or {}
    local buttonRadius = config.buttonRadius or TowerGridPanel.DEFAULT_BUTTON_RADIUS
    local onTowerUpgradesButtonClicked = config.onTowerUpgradesButtonClicked

    local windowHeight = _GetWindowHeight(host)

    local mainButtonSize = config.mainButtonSize or math.floor(windowHeight * 0.15 * 0.8)
    local panelHeight = math.floor(mainButtonSize * 1.25)
    local smallButtonSize = windowHeight * 0.07
    local panelWidth = mainButtonSize * 2 + 95
    local gridHeader = smallButtonSize * 0.5
    local gridWidth = smallButtonSize * 3 + (smallButtonSize * 0.5) * 3
    local gridHeight = smallButtonSize * 3 + (smallButtonSize * 0.5) * 3 + gridHeader

    local obj = {
        host = host,
        buttonRadius = buttonRadius,
        panelHeight = panelHeight,
        mainButtonSize = mainButtonSize,
        smallButtonSize = smallButtonSize,
        panelWidth = panelWidth,
        gridHeader = gridHeader,
        gridWidth = gridWidth,
        gridHeight = gridHeight,
        createTowerButtons = {},
        backgroundRect = nil,
        rowLayout = nil,
        createObjectButton = nil,
        towerUpgradesButton = nil,
        removeObjectButton = nil,
        barrierManagementButton = nil,
        gridBackground = nil,
        gridBackgroundHeader = nil,
        gridLayout = nil,
        closeTowerCreatePanelButton = nil,
        hideCreatePanel = nil,
        backgroundTile = nil,
        widgetName = "",
        luaProxiesReadyCallback = nil,
        isPlacementMode = false
    }

    self.__index = self

    -- Create widgets
    obj.backgroundRect = UiRectangle:new(host)
    overlay:addWidget(obj.backgroundRect)

    obj.rowLayout = UiRowLayout:new(host, "TowerGridPanelRowLayout")
    overlay:addWidget(obj.rowLayout)

    obj.createObjectButton = ImageButton:new(host, overlay, "CreateObjectButton")
    overlay:addCompoundWidget(obj.createObjectButton)

    obj.towerUpgradesButton = ImageButton:new(host, overlay, "TowerUpgradesButton")
    overlay:addCompoundWidget(obj.towerUpgradesButton)

    obj.gridBackground = UiRectangle:new(host, "GridBackground")
    overlay:addWidget(obj.gridBackground)

    obj.gridBackgroundHeader = UiRectangle:new(host, "GridBackgroundHeader")
    overlay:addWidget(obj.gridBackgroundHeader)

    obj.gridLayout = UiGridLayout:new(host, "CreateMenuDropDownGridLayout")
    overlay:addWidget(obj.gridLayout)

    for i = MissileType.BOMB, MissileType.FREEZING_RAY do
        local button = ImageButton:new(host, overlay, "CreateTowerButton" .. tostring(i))
        overlay:addCompoundWidget(button)
        obj.createTowerButtons[#obj.createTowerButtons + 1] = button
        local missileType = MissileTypes.nameByValue(i)

        button:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                button:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                button:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end)

        button:subscribeOnMouseInputClickedCallback(function()
            obj.isPlacementMode = true
            obj.hideCreatePanel()
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "CombatLevelEvents", json.encode(
                                                          {
                    action = "ghost_tower_visibility",
                    tower_type = tostring(missileType),
                    visible = true
                }))
        end)
    end

    obj.barrierManagementButton = ImageButton:new(host, overlay, "BarrierManagementButton")
    overlay:addCompoundWidget(obj.barrierManagementButton)

    obj.removeObjectButton = ImageButton:new(host, overlay, "RemoveObjectButton")
    overlay:addCompoundWidget(obj.removeObjectButton)

    obj.closeTowerCreatePanelButton = UiImageButton:new(host, overlay, "closeTowerCreatePanelButton")
    overlay:addCompoundWidget(obj.closeTowerCreatePanelButton)

    -- hideCreatePanel function
    obj.hideCreatePanel = function() obj.gridBackground:setIsVisible(false) end

    -- Event handlers
    obj.createObjectButton:subscribeOnMouseInputClickedCallback(function()
        obj.isPlacementMode = false
        obj.gridBackground:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = false}))
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "barrier_placement_visibility", visible = false}))
    end)

    obj.createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.createObjectButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            obj.createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    obj.towerUpgradesButton:subscribeOnMouseInputClickedCallback(function()
        obj.hideCreatePanel()
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        if onTowerUpgradesButtonClicked then onTowerUpgradesButtonClicked() end
    end)

    obj.towerUpgradesButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.towerUpgradesButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            obj.towerUpgradesButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    obj.closeTowerCreatePanelButton:subscribeOnMouseInputClickedCallback(function()
        obj.isPlacementMode = false
        obj.hideCreatePanel()
        obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "ghost_tower_visibility", visible = false}))
    end)

    obj.closeTowerCreatePanelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    obj.removeObjectButton:subscribeOnMouseInputClickedCallback(function()
        obj.isPlacementMode = true
        obj.hideCreatePanel()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = true}))
    end)

    obj.removeObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.removeObjectButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            obj.removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    obj.barrierManagementButton:subscribeOnMouseInputClickedCallback(function()
        obj.isPlacementMode = false
        obj.hideCreatePanel()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "barrier_placement_visibility", visible = true}))
    end)

    obj.barrierManagementButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.barrierManagementButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            obj.barrierManagementButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    obj.backgroundTile = obj.backgroundRect

    return setmetatable(obj, self)
end

function TowerGridPanel:subscribeOnLuaProxiesReady(callback) self.luaProxiesReadyCallback = callback end

function TowerGridPanel:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundRect.widgetName

    if self.luaProxiesReadyCallback ~= nil then self.luaProxiesReadyCallback(self.host) end
end

function TowerGridPanel:onCompoundWidgetInitialize() end

function TowerGridPanel:update(host, deltaTimeSec)
    if not self.gridBackground:getIsVisible() or self.isPlacementMode then return end

    local mouseX = _GetMouseCursorPositionX(host)
    local mouseY = _GetMouseCursorPositionY(host)
    local windowWidth = _GetWindowWidth(host)

    local gridOffsetX = self.gridBackground:getHorizontalCenterOffset()
    local gridOffsetY = self.gridBackground:getVerticalCenterOffset()

    local gridCenterX = windowWidth / 2 + gridOffsetX
    local gridBottom = self.panelHeight + 50 + gridOffsetY
    local gridTop = gridBottom + self.gridHeight
    local gridLeft = gridCenterX - self.gridWidth / 2
    local gridRight = gridCenterX + self.gridWidth / 2

    if mouseX >= gridLeft and mouseX <= gridRight and mouseY >= gridBottom and mouseY <= gridTop then return end

    local targetOffsetX = mouseX - windowWidth / 2
    local targetOffsetY = mouseY - self.panelHeight - 50 - self.gridHeight / 2

    local smoothSpeed = 5.0
    local t = 1.0 - math.exp(-smoothSpeed * deltaTimeSec)
    local newOffsetX = gridOffsetX + (targetOffsetX - gridOffsetX) * t
    local newOffsetY = gridOffsetY + (targetOffsetY - gridOffsetY) * t

    self.gridBackground:setHorizontalCenterOffset(math.floor(newOffsetX + 0.5))
    self.gridBackground:setVerticalCenterOffset(math.floor(newOffsetY + 0.5))
end

function TowerGridPanel:setupLayout(canvasName)
    local host = self.host
    local buttonRadius = self.buttonRadius
    local panelHeight = self.panelHeight
    local mainButtonSize = self.mainButtonSize
    local smallButtonSize = self.smallButtonSize
    local panelWidth = self.panelWidth
    local gridHeader = self.gridHeader
    local gridWidth = self.gridWidth
    local gridHeight = self.gridHeight

    self.backgroundRect:setParent(host, canvasName, canvasName)
    self.backgroundRect:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                  canvasName)
    self.backgroundRect:setWidth(panelWidth)
    self.backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvasName, 0)
    self.backgroundRect:setColorHexValue(Styles.Colors.panelColor)
    self.backgroundRect:setZOrder(1)
    self.backgroundRect:setHeight(panelHeight)
    self.backgroundRect:setBorderRadius(buttonRadius)

    self.rowLayout:setParent(host, canvasName, self.backgroundRect.widgetName)
    self.rowLayout:fill(self.backgroundRect.widgetName)
    self.rowLayout:setSpacing(45)
    self.rowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)

    self.createObjectButton:setParent(host, canvasName, self.rowLayout.widgetName)
    self.createObjectButton:setWidth(mainButtonSize)
    self.createObjectButton:setHeight(mainButtonSize)
    self.createObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.createObjectButton:setButtonBorderRadius(buttonRadius)
    self.createObjectButton:setImageTextureSource("hammer.png")
    self.createObjectButton:setRotationDegrees(180)

    self.towerUpgradesButton:setParent(host, canvasName, self.rowLayout.widgetName)
    self.towerUpgradesButton:setWidth(mainButtonSize)
    self.towerUpgradesButton:setHeight(mainButtonSize)
    self.towerUpgradesButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.towerUpgradesButton:setButtonBorderRadius(buttonRadius)
    self.towerUpgradesButton:setImageTextureSource("blueprint.png")

    self.gridBackground:setParent(host, canvasName, canvasName)
    self.gridBackground:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.TOP,
                                  self.backgroundRect.widgetName, 50)
    self.gridBackground:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                  self.backgroundRect.widgetName)
    self.gridBackground:setWidth(gridWidth)
    self.gridBackground:setHeight(gridHeight)
    self.gridBackground:setColorHexValue(Styles.Colors.panelColor)
    self.gridBackground:setBorderRadius(buttonRadius)
    self.gridBackground:setIsVisible(false)
    self.gridBackground:setZOrder(2)

    self.gridBackgroundHeader:setParent(host, canvasName, self.gridBackground.widgetName)
    self.gridBackgroundHeader:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                        self.gridBackground.widgetName)
    self.gridBackgroundHeader:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                        UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.gridBackground.widgetName)
    self.gridBackgroundHeader:setWidth(gridWidth)
    self.gridBackgroundHeader:setHeight(gridHeader)
    self.gridBackgroundHeader:setColorHexValue(Styles.Colors.headerPanelColor)
    self.gridBackgroundHeader:setBorderRadius(buttonRadius)
    self.gridBackgroundHeader:setIsRoundBottom(false)
    self.gridBackgroundHeader:setZOrder(3)

    self.gridLayout:setParent(host, canvasName, self.gridBackground.widgetName)
    self.gridLayout:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                              self.gridBackgroundHeader.widgetName, 0)
    self.gridLayout:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.gridBackground.widgetName)
    self.gridLayout:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                              self.gridBackground.widgetName)
    self.gridLayout:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                              self.gridBackground.widgetName)
    self.gridLayout:setZOrder(3)
    self.gridLayout:setHorizontalSpacing(smallButtonSize * 0.5)
    self.gridLayout:setVerticalSpacing(smallButtonSize * 0.5)
    self.gridLayout:setColumnsCount(3)
    self.gridLayout:setRowsCount(3)
    self.gridLayout:setAlignment(UiGridLayout.UiGridHorizontalAlignmentType.CENTER,
                                 UiGridLayout.UiGridVerticalAlignmentType.CENTER)

    for i = 1, #self.createTowerButtons do
        local missileValue = MissileType.BOMB + (i - 1)
        local imageSource = MissileTypes.IconByValue[missileValue] or "space_station_img.png"

        self.createTowerButtons[i]:setParent(host, canvasName, self.gridLayout.widgetName)
        self.createTowerButtons[i]:setWidth(smallButtonSize)
        self.createTowerButtons[i]:setHeight(smallButtonSize)
        self.createTowerButtons[i]:setButtonColorHexValue(Styles.Colors.buttonColor)
        self.createTowerButtons[i]:setButtonBorderRadius(buttonRadius)
        self.createTowerButtons[i]:setImageTextureSource(imageSource)
        self.createTowerButtons[i]:setRotationDegrees(180)
        self.createTowerButtons[i]:setImageFlipped(true)
        self.createTowerButtons[i]:setZOrder(3)
    end

    self.barrierManagementButton:setParent(host, canvasName, self.gridLayout.widgetName)
    self.barrierManagementButton:setWidth(smallButtonSize)
    self.barrierManagementButton:setHeight(smallButtonSize)
    self.barrierManagementButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.barrierManagementButton:setButtonBorderRadius(buttonRadius)
    self.barrierManagementButton:setImageTextureSource("warning.png")

    self.removeObjectButton:setParent(host, canvasName, self.gridLayout.widgetName)
    self.removeObjectButton:setWidth(smallButtonSize)
    self.removeObjectButton:setHeight(smallButtonSize)
    self.removeObjectButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.removeObjectButton:setButtonBorderRadius(buttonRadius)
    self.removeObjectButton:setImageTextureSource("trash.png")
    self.removeObjectButton:setRotationDegrees(180)

    self.closeTowerCreatePanelButton:setParent(host, canvasName, self.gridBackgroundHeader.widgetName)
    self.closeTowerCreatePanelButton:setWidth(smallButtonSize * 0.4)
    self.closeTowerCreatePanelButton:setHeight(smallButtonSize * 0.4)
    self.closeTowerCreatePanelButton:setUseImageCustomColor(true)
    self.closeTowerCreatePanelButton:setImageColorHexValue(0x000000)
    self.closeTowerCreatePanelButton:setImageTextureSource("cancel.png")
    self.closeTowerCreatePanelButton:setIsBackgroundVisible(false)
    self.closeTowerCreatePanelButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                               self.gridBackgroundHeader.widgetName, 0)
    self.closeTowerCreatePanelButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                               self.gridBackgroundHeader.widgetName, 0)
    self.closeTowerCreatePanelButton:setZOrder(4)
end

function TowerGridPanel:handleBroadcastSwitchModeIdle(host)
    self.isPlacementMode = false
    self.hideCreatePanel()
    EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                              json.encode({action = "ghost_tower_visibility", visible = false}))
    EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                              json.encode({action = "barrier_placement_visibility", visible = false}))
end

return TowerGridPanel
