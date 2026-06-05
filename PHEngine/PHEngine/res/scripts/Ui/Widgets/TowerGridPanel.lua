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
local UiLabel = require("Ui/Core/uiLabel")
local UiImage = require("Ui/Core/uiImage")
local UiDivider = require("Ui/Core/uiDivider")
local ImageButton = require("Ui/Widgets/ImageButton")
local UiImageButton = require("Ui/Widgets/ImageButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local MissileTypes = require("Ui/Common/missileTypes")

local MissileType = MissileTypes.MissileType

local FONT = "JetBrainsMono-VariableFont_wght"

-- Placeholder display data for the build-palette cards (mockup parity). TODO: source real display names,
-- DPS and crystal cost from the engine (Game::eMissileType metadata) once exposed to Lua — the values
-- below are hard-coded mockup placeholders.
local TOWER_CARD_INFO = {
    [MissileType.BOMB] = {name = "РАКЕТА", role = "ТЯЖЁЛАЯ", dps = 60, cost = 160, accent = Styles.Combat.danger},
    [MissileType.FREEZING_BOMB] = {name = "КРИО-РАКЕТА", role = "ЗАМЕДЛ.", dps = 28, cost = 180, accent = Styles.Combat.cyan},
    [MissileType.ELECTRO_RAY] = {name = "ЭЛЕКТРО-ЛУЧ", role = "АОЕ", dps = 35, cost = 200, accent = Styles.Combat.lilac},
    [MissileType.BLACK_HOLE] = {name = "ЧЁРНАЯ ДЫРА", role = "ГРАВИТ.", dps = 42, cost = 240, accent = Styles.Combat.indigo},
    [MissileType.FREEZING_RAY] = {name = "КРИО-ЛУЧ", role = "ЗАМЕДЛ.", dps = 22, cost = 150, accent = Styles.Combat.cyanGlow}
}

-- Build-palette section sub-plates ("подплашки", mockup sections "01 · БАШНИ" / "02 · БАРЬЕРЫ").
local TOWER_SECTION = {num = "01", name = "БАШНИ"}
-- Barrier build-palette section. Only the force-field barrier is exposed.
-- Cost is a mockup placeholder (mirrors CB_BARRIERS_PAL.shield) until sourced from the engine.
local BARRIER_SECTION = {num = "02", name = "БАРЬЕРЫ"}
local BARRIER_CARD_INFO = {name = "СИЛОВОЕ ПОЛЕ", cost = 75, accent = Styles.Combat.lilac, icon = "shield.png"}

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

    -- Build-palette popup is now a vertical card list (one card per tower) plus a tools row.
    local cardWidth = 264
    local cardHeight = 54
    local cardSpacing = 8
    local cardPad = 12
    local towerCount = MissileType.FREEZING_RAY - MissileType.BOMB + 1
    local gridHeader = 32
    local sectionHeader = 22 -- "02 · БАРЬЕРЫ" sub-plate strip
    local sectionGap = 12 -- extra gap separating the towers list from the barriers section
    local gridWidth = cardWidth + cardPad * 2
    -- towers section (sub-header + cards) + barriers section (sub-header + a single card)
    local gridHeight = gridHeader + cardPad + sectionHeader + cardSpacing + towerCount * cardHeight +
                           (towerCount - 1) * cardSpacing + sectionGap + sectionHeader + cardSpacing + cardHeight +
                           cardPad

    local obj = {
        host = host,
        buttonRadius = buttonRadius,
        panelHeight = panelHeight,
        mainButtonSize = mainButtonSize,
        smallButtonSize = smallButtonSize,
        panelWidth = panelWidth,
        cardWidth = cardWidth,
        cardHeight = cardHeight,
        cardSpacing = cardSpacing,
        cardPad = cardPad,
        gridHeader = gridHeader,
        sectionHeader = sectionHeader,
        sectionGap = sectionGap,
        gridWidth = gridWidth,
        gridHeight = gridHeight,
        towerCards = {},
        towerSectionHeader = nil,
        towerSectionLabel = nil,
        towerSectionCountLabel = nil,
        towerSectionDivider = nil,
        barrierSectionHeader = nil,
        barrierSectionLabel = nil,
        barrierSectionCountLabel = nil,
        barrierSectionDivider = nil,
        barrierCard = nil,
        backgroundRect = nil,
        rowLayout = nil,
        createObjectButton = nil,
        towerUpgradesButton = nil,
        gridBackground = nil,
        gridBackgroundHeader = nil,
        gridTitleLabel = nil,
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

    obj.gridTitleLabel = UiLabel:new(host, FONT, "GridTitleLabel")
    overlay:addWidget(obj.gridTitleLabel)

    -- Towers section sub-plate ("подплашка") above the tower cards.
    obj.towerSectionHeader = UiRectangle:new(host, "TowerSectionHeader")
    overlay:addWidget(obj.towerSectionHeader)

    obj.towerSectionLabel = UiLabel:new(host, FONT, "TowerSectionLabel")
    overlay:addWidget(obj.towerSectionLabel)

    obj.towerSectionCountLabel = UiLabel:new(host, FONT, "TowerSectionCount")
    overlay:addWidget(obj.towerSectionCountLabel)

    obj.towerSectionDivider = UiDivider:new(host, "TowerSectionDivider")
    overlay:addWidget(obj.towerSectionDivider)

    -- One card per tower type. The card background (a plain rectangle) is the clickable / hoverable
    -- surface; the icon and text labels sit on top as non-interactive children.
    for i = MissileType.BOMB, MissileType.FREEZING_RAY do
        local missileType = MissileTypes.nameByValue(i)
        local card = {
            missileType = missileType,
            info = TOWER_CARD_INFO[i],
            bg = UiRectangle:new(host, "TowerCardBg" .. tostring(i)),
            accentBar = UiRectangle:new(host, "TowerCardAccent" .. tostring(i)),
            icon = UiImage:new(host, "TowerCardIcon" .. tostring(i)),
            nameLabel = UiLabel:new(host, FONT, "TowerCardName" .. tostring(i)),
            dpsLabel = UiLabel:new(host, FONT, "TowerCardDps" .. tostring(i)),
            costLabel = UiLabel:new(host, FONT, "TowerCardCost" .. tostring(i))
        }
        overlay:addWidget(card.bg)
        overlay:addWidget(card.accentBar)
        overlay:addWidget(card.icon)
        overlay:addWidget(card.nameLabel)
        overlay:addWidget(card.dpsLabel)
        overlay:addWidget(card.costLabel)
        obj.towerCards[#obj.towerCards + 1] = card

        card.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if card.locked then return end -- not affordable: keep dimmed, no hover highlight
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                card.bg:setColorHexValue(Styles.Combat.chipHoverColor)
            else
                card.bg:setColorHexValue(Styles.Combat.chipColor)
            end
        end)

        card.bg:subscribeOnMouseInputClickedCallback(function()
            if card.locked then return end -- not affordable: ignore clicks
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

    -- Barriers section sub-plate ("подплашка") + a single force-field card.
    obj.barrierSectionHeader = UiRectangle:new(host, "BarrierSectionHeader")
    overlay:addWidget(obj.barrierSectionHeader)

    obj.barrierSectionLabel = UiLabel:new(host, FONT, "BarrierSectionLabel")
    overlay:addWidget(obj.barrierSectionLabel)

    obj.barrierSectionCountLabel = UiLabel:new(host, FONT, "BarrierSectionCount")
    overlay:addWidget(obj.barrierSectionCountLabel)

    obj.barrierSectionDivider = UiDivider:new(host, "BarrierSectionDivider")
    overlay:addWidget(obj.barrierSectionDivider)

    obj.barrierCard = {
        info = BARRIER_CARD_INFO,
        bg = UiRectangle:new(host, "BarrierCardBg"),
        accentBar = UiRectangle:new(host, "BarrierCardAccent"),
        icon = UiImage:new(host, "BarrierCardIcon"),
        nameLabel = UiLabel:new(host, FONT, "BarrierCardName"),
        costLabel = UiLabel:new(host, FONT, "BarrierCardCost")
    }
    overlay:addWidget(obj.barrierCard.bg)
    overlay:addWidget(obj.barrierCard.accentBar)
    overlay:addWidget(obj.barrierCard.icon)
    overlay:addWidget(obj.barrierCard.nameLabel)
    overlay:addWidget(obj.barrierCard.costLabel)

    obj.barrierCard.bg:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if obj.barrierCard.locked then return end -- not affordable: keep dimmed, no hover highlight
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.barrierCard.bg:setColorHexValue(Styles.Combat.chipHoverColor)
        else
            obj.barrierCard.bg:setColorHexValue(Styles.Combat.chipColor)
        end
    end)

    obj.barrierCard.bg:subscribeOnMouseInputClickedCallback(function()
        if obj.barrierCard.locked then return end -- not affordable: ignore clicks
        obj.isPlacementMode = true
        obj.hideCreatePanel()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "barrier_placement_visibility", visible = true}))
    end)

    obj.closeTowerCreatePanelButton = UiImageButton:new(host, overlay, "closeTowerCreatePanelButton")
    overlay:addCompoundWidget(obj.closeTowerCreatePanelButton)

    -- show / hide the tower-create popup grid
    obj.hideCreatePanel = function() obj.gridBackground:setIsVisible(false) end
    obj.showCreatePanel = function()
        obj.isPlacementMode = false
        obj.gridBackground:setIsVisible(true)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode(
                                                      {action = "remove_tower_marker_visibility", visible = false}))
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "barrier_placement_visibility", visible = false}))
    end
    obj.isCreatePanelVisible = function() return obj.gridBackground:getIsVisible() end
    obj.toggleCreatePanel = function()
        if obj.gridBackground:getIsVisible() then obj.hideCreatePanel() else obj.showCreatePanel() end
    end

    -- Event handlers
    obj.createObjectButton:subscribeOnMouseInputClickedCallback(function() obj.showCreatePanel() end)

    obj.createObjectButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.createObjectButton:setButtonColorHexValue(Styles.Combat.chipHoverColor)
        else
            obj.createObjectButton:setButtonColorHexValue(Styles.Combat.chipColor)
        end
    end)

    obj.towerUpgradesButton:subscribeOnMouseInputClickedCallback(function()
        obj.hideCreatePanel()
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        if onTowerUpgradesButtonClicked then onTowerUpgradesButtonClicked() end
    end)

    obj.towerUpgradesButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.towerUpgradesButton:setButtonColorHexValue(Styles.Combat.chipHoverColor)
        else
            obj.towerUpgradesButton:setButtonColorHexValue(Styles.Combat.chipColor)
        end
    end)

    obj.closeTowerCreatePanelButton:subscribeOnMouseInputClickedCallback(function()
        obj.isPlacementMode = false
        obj.hideCreatePanel()
        obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Combat.chipColor)
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "ghost_tower_visibility", visible = false}))
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY, "CombatLevelEvents",
                                                  json.encode({action = "barrier_placement_visibility", visible = false}))
    end)

    obj.closeTowerCreatePanelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Combat.chipHoverColor)
        else
            obj.closeTowerCreatePanelButton:setButtonColorHexValue(Styles.Combat.chipColor)
        end
    end)

    obj.backgroundTile = obj.backgroundRect

    return setmetatable(obj, self)
end

function TowerGridPanel:subscribeOnLuaProxiesReady(callback) self.luaProxiesReadyCallback = callback end

-- Toggle the bottom control bar (hammer / blueprint). Children follow the parent's visibility, so hiding
-- the background tile hides the whole bar. The create popup is anchored to the canvas, not this bar, so it
-- is unaffected.
function TowerGridPanel:setControlBarVisible(isVisible)
    self.backgroundRect:setIsVisible(isVisible)
end

function TowerGridPanel:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundRect.widgetName

    if self.luaProxiesReadyCallback ~= nil then self.luaProxiesReadyCallback(self.host) end
end

function TowerGridPanel:onCompoundWidgetInitialize() end

-- The create popup stays at its anchored position (centered above the bottom panel); it no longer
-- follows the cursor.
function TowerGridPanel:update(host, deltaTimeSec) end

-- Dim + lock the tower cards the player cannot currently afford. Called by the overlay whenever the
-- crystal count changes.
local function updateCardAffordability(card, crystalsCount)
    local locked = card.info.cost > crystalsCount
    if card.locked == locked then return end
    card.locked = locked
    local opacity = locked and 0.4 or 1.0
    card.bg:setOpacity(opacity)
    card.accentBar:setOpacity(opacity)
    card.icon:setOpacity(opacity)
    card.nameLabel:setOpacity(opacity)
    if card.dpsLabel then card.dpsLabel:setOpacity(opacity) end
    card.costLabel:setOpacity(opacity)
    card.costLabel:setTextColorHexValue(locked and Styles.Combat.danger or Styles.Combat.cyanGlow)
end

function TowerGridPanel:updateAffordability(crystalsCount)
    for _, card in ipairs(self.towerCards) do updateCardAffordability(card, crystalsCount) end
    updateCardAffordability(self.barrierCard, crystalsCount)
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
    self.backgroundRect:setColorHexValue(Styles.Combat.panelColor)
    self.backgroundRect:setOpacity(Styles.Combat.panelOpacity)
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
    self.createObjectButton:setButtonColorHexValue(Styles.Combat.chipColor)
    self.createObjectButton:setButtonBorderRadius(buttonRadius)
    self.createObjectButton:setImageTextureSource("hammer.png")
    self.createObjectButton:setRotationDegrees(180)

    self.towerUpgradesButton:setParent(host, canvasName, self.rowLayout.widgetName)
    self.towerUpgradesButton:setWidth(mainButtonSize)
    self.towerUpgradesButton:setHeight(mainButtonSize)
    self.towerUpgradesButton:setButtonColorHexValue(Styles.Combat.chipColor)
    self.towerUpgradesButton:setButtonBorderRadius(buttonRadius)
    self.towerUpgradesButton:setImageTextureSource("blueprint.png")

    self.gridBackground:setParent(host, canvasName, canvasName)
    -- Arsenal palette is docked to the left edge (mockup left shell), offset to clear the side icon dock.
    self.gridBackground:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, canvasName, 120)
    self.gridBackground:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                  canvasName, 0)
    self.gridBackground:setWidth(gridWidth)
    self.gridBackground:setHeight(gridHeight)
    self.gridBackground:setColorHexValue(Styles.Combat.panelColor)
    self.gridBackground:setOpacity(Styles.Combat.panelOpacity)
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
    self.gridBackgroundHeader:setColorHexValue(Styles.Combat.panelHeaderColor)
    self.gridBackgroundHeader:setBorderRadius(buttonRadius)
    self.gridBackgroundHeader:setIsRoundBottom(false)
    self.gridBackgroundHeader:setZOrder(3)

    -- Header title.
    self.gridTitleLabel:setParent(host, canvasName, self.gridBackgroundHeader.widgetName)
    self.gridTitleLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                  self.gridBackgroundHeader.widgetName, 12)
    self.gridTitleLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                  self.gridBackgroundHeader.widgetName, 0)
    self.gridTitleLabel:setWidth(gridWidth - 50)
    self.gridTitleLabel:setHeight(gridHeader)
    self.gridTitleLabel:setFontSize(12)
    self.gridTitleLabel:setTextColorHexValue(Styles.Combat.cyanGlow)
    self.gridTitleLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
    self.gridTitleLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.gridTitleLabel:setText("АРСЕНАЛ · СИСТЕМЫ ОБОРОНЫ")
    self.gridTitleLabel:setZOrder(4)

    -- Tower cards (vertical list). Children are not toggled directly — they inherit the popup's
    -- visibility via the gridBackground parent.
    local cardWidth = self.cardWidth
    local cardHeight = self.cardHeight
    local cardSpacing = self.cardSpacing
    local cardPad = self.cardPad

    -- Towers sub-plate header ("01 · БАШНИ"), docked below the main arsenal header.
    self.towerSectionHeader:setParent(host, canvasName, self.gridBackground.widgetName)
    self.towerSectionHeader:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                      UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.gridBackground.widgetName)
    self.towerSectionHeader:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                      self.gridBackgroundHeader.widgetName, cardPad)
    self.towerSectionHeader:setWidth(cardWidth)
    self.towerSectionHeader:setHeight(self.sectionHeader)
    self.towerSectionHeader:setColorHexValue(Styles.Combat.panelHeaderColor)
    self.towerSectionHeader:setBorderRadius(buttonRadius)
    self.towerSectionHeader:setZOrder(3)

    self.towerSectionLabel:setParent(host, canvasName, self.towerSectionHeader.widgetName)
    self.towerSectionLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                     self.towerSectionHeader.widgetName, 10)
    self.towerSectionLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                     self.towerSectionHeader.widgetName, 0)
    self.towerSectionLabel:setWidth(cardWidth - 50)
    self.towerSectionLabel:setHeight(self.sectionHeader)
    self.towerSectionLabel:setFontSize(11)
    self.towerSectionLabel:setTextColorHexValue(Styles.Combat.cyanGlow)
    self.towerSectionLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
    self.towerSectionLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.towerSectionLabel:setText(TOWER_SECTION.num .. " · " .. TOWER_SECTION.name)
    self.towerSectionLabel:setZOrder(4)

    self.towerSectionCountLabel:setParent(host, canvasName, self.towerSectionHeader.widgetName)
    self.towerSectionCountLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                          self.towerSectionHeader.widgetName, 12)
    self.towerSectionCountLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                          UiItemBase.UiAnchorType.VERTICAL_CENTER, self.towerSectionHeader.widgetName, 0)
    self.towerSectionCountLabel:setWidth(40)
    self.towerSectionCountLabel:setHeight(self.sectionHeader)
    self.towerSectionCountLabel:setFontSize(11)
    self.towerSectionCountLabel:setTextColorHexValue(Styles.Combat.textDim)
    self.towerSectionCountLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
    self.towerSectionCountLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.towerSectionCountLabel:setText(string.format("%02d", #self.towerCards))
    self.towerSectionCountLabel:setZOrder(4)

    -- Faded divider under the towers heading (mockup .cb-palette section separators).
    self.towerSectionDivider:setParent(host, canvasName, self.gridBackground.widgetName)
    self.towerSectionDivider:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                       UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.gridBackground.widgetName)
    self.towerSectionDivider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                       self.towerSectionHeader.widgetName, 2)
    self.towerSectionDivider:setWidth(cardWidth)
    self.towerSectionDivider:setHeight(4)
    self.towerSectionDivider:setColorHexValue(Styles.Combat.cyan)
    self.towerSectionDivider:setOpacity(0.4)
    self.towerSectionDivider:setLineWidthPx(0.75)
    self.towerSectionDivider:setEdgeFade(0.12)
    self.towerSectionDivider:setZOrder(4)

    for i = 1, #self.towerCards do
        local card = self.towerCards[i]
        local info = card.info
        local missileValue = MissileType.BOMB + (i - 1)
        local imageSource = MissileTypes.IconByValue[missileValue] or "space_station_img.png"

        card.bg:setParent(host, canvasName, self.gridBackground.widgetName)
        card.bg:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                          self.gridBackground.widgetName)
        if i == 1 then
            card.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                              self.towerSectionHeader.widgetName, cardSpacing)
        else
            card.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                              self.towerCards[i - 1].bg.widgetName, cardSpacing)
        end
        card.bg:setWidth(cardWidth)
        card.bg:setHeight(cardHeight)
        card.bg:setColorHexValue(Styles.Combat.chipColor)
        card.bg:setBorderRadius(buttonRadius)
        card.bg:setZOrder(3)
        card.bg:enableMouseInputReceiverBase(host)
        card.bg:setIfCanInterceptMouseInputEvent(true)

        card.accentBar:setParent(host, canvasName, card.bg.widgetName)
        card.accentBar:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, card.bg.widgetName, 0)
        card.accentBar:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, card.bg.widgetName, 0)
        card.accentBar:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, card.bg.widgetName, 0)
        card.accentBar:setWidth(4)
        card.accentBar:setColorHexValue(info.accent)
        card.accentBar:setZOrder(4)

        card.icon:setParent(host, canvasName, card.bg.widgetName)
        card.icon:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, card.bg.widgetName, 16)
        card.icon:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                            card.bg.widgetName, 0)
        card.icon:setWidth(30)
        card.icon:setHeight(30)
        card.icon:setTextureSource(imageSource)
        card.icon:setZOrder(4)

        card.nameLabel:setParent(host, canvasName, card.bg.widgetName)
        card.nameLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, card.bg.widgetName, 58)
        card.nameLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, card.bg.widgetName, 9)
        card.nameLabel:setWidth(cardWidth - 130)
        card.nameLabel:setHeight(18)
        card.nameLabel:setFontSize(14)
        card.nameLabel:setTextColorHexValue(Styles.Combat.textBright)
        card.nameLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        card.nameLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        card.nameLabel:setText(info.name)
        card.nameLabel:setZOrder(4)

        card.dpsLabel:setParent(host, canvasName, card.bg.widgetName)
        card.dpsLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, card.bg.widgetName, 58)
        card.dpsLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, card.bg.widgetName, 9)
        card.dpsLabel:setWidth(cardWidth - 130)
        card.dpsLabel:setHeight(14)
        card.dpsLabel:setFontSize(10)
        card.dpsLabel:setTextColorHexValue(Styles.Combat.textDim)
        card.dpsLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        card.dpsLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        card.dpsLabel:setText("DPS " .. tostring(info.dps) .. " · " .. info.role)
        card.dpsLabel:setZOrder(4)

        card.costLabel:setParent(host, canvasName, card.bg.widgetName)
        card.costLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, card.bg.widgetName, 14)
        card.costLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                 card.bg.widgetName, 0)
        card.costLabel:setWidth(60)
        card.costLabel:setHeight(18)
        card.costLabel:setFontSize(13)
        card.costLabel:setTextColorHexValue(Styles.Combat.cyanGlow)
        card.costLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
        card.costLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        card.costLabel:setText(tostring(info.cost))
        card.costLabel:setZOrder(4)
    end

    -- Barriers sub-plate header ("02 · БАРЬЕРЫ"), docked below the last tower card.
    local lastTowerBg = self.towerCards[#self.towerCards].bg.widgetName
    self.barrierSectionHeader:setParent(host, canvasName, self.gridBackground.widgetName)
    self.barrierSectionHeader:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                        UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.gridBackground.widgetName)
    self.barrierSectionHeader:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, lastTowerBg,
                                        self.sectionGap)
    self.barrierSectionHeader:setWidth(cardWidth)
    self.barrierSectionHeader:setHeight(self.sectionHeader)
    self.barrierSectionHeader:setColorHexValue(Styles.Combat.panelHeaderColor)
    self.barrierSectionHeader:setBorderRadius(buttonRadius)
    self.barrierSectionHeader:setZOrder(3)

    self.barrierSectionLabel:setParent(host, canvasName, self.barrierSectionHeader.widgetName)
    self.barrierSectionLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                       self.barrierSectionHeader.widgetName, 10)
    self.barrierSectionLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                       UiItemBase.UiAnchorType.VERTICAL_CENTER, self.barrierSectionHeader.widgetName, 0)
    self.barrierSectionLabel:setWidth(cardWidth - 50)
    self.barrierSectionLabel:setHeight(self.sectionHeader)
    self.barrierSectionLabel:setFontSize(11)
    self.barrierSectionLabel:setTextColorHexValue(Styles.Combat.lilac)
    self.barrierSectionLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
    self.barrierSectionLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.barrierSectionLabel:setText(BARRIER_SECTION.num .. " · " .. BARRIER_SECTION.name)
    self.barrierSectionLabel:setZOrder(4)

    self.barrierSectionCountLabel:setParent(host, canvasName, self.barrierSectionHeader.widgetName)
    self.barrierSectionCountLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                            self.barrierSectionHeader.widgetName, 12)
    self.barrierSectionCountLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                            UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                            self.barrierSectionHeader.widgetName, 0)
    self.barrierSectionCountLabel:setWidth(40)
    self.barrierSectionCountLabel:setHeight(self.sectionHeader)
    self.barrierSectionCountLabel:setFontSize(11)
    self.barrierSectionCountLabel:setTextColorHexValue(Styles.Combat.textDim)
    self.barrierSectionCountLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
    self.barrierSectionCountLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.barrierSectionCountLabel:setText("01")
    self.barrierSectionCountLabel:setZOrder(4)

    -- Faded divider under the barriers heading (matches the towers one).
    self.barrierSectionDivider:setParent(host, canvasName, self.gridBackground.widgetName)
    self.barrierSectionDivider:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                                         UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.gridBackground.widgetName)
    self.barrierSectionDivider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                         self.barrierSectionHeader.widgetName, 2)
    self.barrierSectionDivider:setWidth(cardWidth)
    self.barrierSectionDivider:setHeight(4)
    self.barrierSectionDivider:setColorHexValue(Styles.Combat.lilac)
    self.barrierSectionDivider:setOpacity(0.4)
    self.barrierSectionDivider:setLineWidthPx(0.75)
    self.barrierSectionDivider:setEdgeFade(0.12)
    self.barrierSectionDivider:setZOrder(4)

    -- Force-field barrier card.
    local barrierCard = self.barrierCard
    local barrierInfo = barrierCard.info
    barrierCard.bg:setParent(host, canvasName, self.gridBackground.widgetName)
    barrierCard.bg:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                             self.gridBackground.widgetName)
    barrierCard.bg:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                             self.barrierSectionHeader.widgetName, cardSpacing)
    barrierCard.bg:setWidth(cardWidth)
    barrierCard.bg:setHeight(cardHeight)
    barrierCard.bg:setColorHexValue(Styles.Combat.chipColor)
    barrierCard.bg:setBorderRadius(buttonRadius)
    barrierCard.bg:setZOrder(3)
    barrierCard.bg:enableMouseInputReceiverBase(host)
    barrierCard.bg:setIfCanInterceptMouseInputEvent(true)

    barrierCard.accentBar:setParent(host, canvasName, barrierCard.bg.widgetName)
    barrierCard.accentBar:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                    barrierCard.bg.widgetName, 0)
    barrierCard.accentBar:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, barrierCard.bg.widgetName,
                                    0)
    barrierCard.accentBar:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                    barrierCard.bg.widgetName, 0)
    barrierCard.accentBar:setWidth(4)
    barrierCard.accentBar:setColorHexValue(barrierInfo.accent)
    barrierCard.accentBar:setZOrder(4)

    barrierCard.icon:setParent(host, canvasName, barrierCard.bg.widgetName)
    barrierCard.icon:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, barrierCard.bg.widgetName, 16)
    barrierCard.icon:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               barrierCard.bg.widgetName, 0)
    barrierCard.icon:setWidth(30)
    barrierCard.icon:setHeight(30)
    barrierCard.icon:setTextureSource(barrierInfo.icon)
    barrierCard.icon:setRotationDegrees(180)
    barrierCard.icon:setUseImageCustomColor(true)
    barrierCard.icon:setColorHexValue(Styles.Combat.lilac)
    barrierCard.icon:setZOrder(4)

    barrierCard.nameLabel:setParent(host, canvasName, barrierCard.bg.widgetName)
    barrierCard.nameLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                    barrierCard.bg.widgetName, 58)
    barrierCard.nameLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                    barrierCard.bg.widgetName, 0)
    barrierCard.nameLabel:setWidth(cardWidth - 130)
    barrierCard.nameLabel:setHeight(18)
    barrierCard.nameLabel:setFontSize(14)
    barrierCard.nameLabel:setTextColorHexValue(Styles.Combat.textBright)
    barrierCard.nameLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
    barrierCard.nameLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    barrierCard.nameLabel:setText(barrierInfo.name)
    barrierCard.nameLabel:setZOrder(4)

    barrierCard.costLabel:setParent(host, canvasName, barrierCard.bg.widgetName)
    barrierCard.costLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                    barrierCard.bg.widgetName, 14)
    barrierCard.costLabel:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                                    barrierCard.bg.widgetName, 0)
    barrierCard.costLabel:setWidth(60)
    barrierCard.costLabel:setHeight(18)
    barrierCard.costLabel:setFontSize(13)
    barrierCard.costLabel:setTextColorHexValue(Styles.Combat.cyanGlow)
    barrierCard.costLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
    barrierCard.costLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    barrierCard.costLabel:setText(tostring(barrierInfo.cost))
    barrierCard.costLabel:setZOrder(4)

    self.closeTowerCreatePanelButton:setParent(host, canvasName, self.gridBackgroundHeader.widgetName)
    self.closeTowerCreatePanelButton:setWidth(smallButtonSize * 0.4)
    self.closeTowerCreatePanelButton:setHeight(smallButtonSize * 0.4)
    self.closeTowerCreatePanelButton:setUseImageCustomColor(true)
    self.closeTowerCreatePanelButton:setImageColorHexValue(Styles.Combat.textBright)
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
