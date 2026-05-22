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
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local UiRectangle = require("Ui/Core/uiRectangle")
local UiScrollList = require("Ui/Core/uiScrollList")
local UiItem = require("Ui/Core/uiItem")
local UiLabel = require("Ui/Core/uiLabel")
local ImageButton = require("Ui/Widgets/ImageButton")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")
local EventsHelper = require("Ui/Core/eventsHelper")
local UpgradeIcon = require("Ui/Core/uiUpgradeIcon")
local ConnectionLine = require("Ui/Core/uiConnectionLine")

TowerUpgradesPanel = {}

local treeNodes = {
    {
        id = "he_rocket",
        name = "Фугасная Ракета",
        texture = "he_rocket.png",
        color = 0xe85d04,
        glow = 0xff8c38,
        x = 50,
        y = 6,
        parents = {}
    }, {
        id = "ice_rocket",
        name = "Ледяная Ракета",
        texture = "ice_rocket.png",
        color = 0x48cae4,
        glow = 0x90e0ef,
        x = 14,
        y = 24,
        parents = {"he_rocket"}
    }, {
        id = "repair_beam",
        name = "Ремонтный Луч",
        texture = "repair_beam.png",
        color = 0x22c55e,
        glow = 0x86efac,
        x = 50,
        y = 24,
        parents = {"he_rocket"}
    }, {
        id = "plasma_rocket",
        name = "Плазменная Ракета",
        texture = "plasma_rocket.png",
        color = 0xf72585,
        glow = 0xff49a0,
        x = 86,
        y = 24,
        parents = {"he_rocket"}
    }, {
        id = "ice_beam",
        name = "Ледяной Луч",
        texture = "ice_beam.png",
        color = 0x00b4d8,
        glow = 0x48cae4,
        x = 8,
        y = 45,
        parents = {"ice_rocket"}
    }, {
        id = "electric_beam",
        name = "Электрический Луч",
        texture = "electric_beam.png",
        color = 0x7b2d8b,
        glow = 0xb44fc4,
        x = 31,
        y = 45,
        parents = {"plasma_rocket", "ice_rocket"}
    }, {
        id = "nano_beam",
        name = "Нано-Луч",
        texture = "nano_beam.png",
        color = 0x10b981,
        glow = 0x6ee7b7,
        x = 62,
        y = 45,
        parents = {"repair_beam"}
    }, {
        id = "cryo_cannon",
        name = "Крио-Пушка",
        texture = "cryo_cannon.png",
        color = 0x023e8a,
        glow = 0x0077b6,
        x = 88,
        y = 45,
        parents = {"plasma_rocket"}
    }, {
        id = "gravity_bomb",
        name = "Гравитационная Бомба",
        texture = "gravity_bomb.png",
        color = 0x7209b7,
        glow = 0xb44fc4,
        x = 30,
        y = 66,
        parents = {"ice_beam", "electric_beam"}
    }, {
        id = "force_barrier",
        name = "Форс-Барьер",
        texture = "force_barrier.png",
        color = 0xf59e0b,
        glow = 0xfcd34d,
        x = 72,
        y = 66,
        parents = {"nano_beam", "cryo_cannon"}
    }, {
        id = "quantum_nexus",
        name = "Квантовый Нексус",
        texture = "quantum_nexus.png",
        color = 0x818cf8,
        glow = 0xe0e7ff,
        x = 50,
        y = 85,
        parents = {"gravity_bomb", "force_barrier"}
    }
}

local nodeById = {}
for _, node in ipairs(treeNodes) do nodeById[node.id] = node end

local connections = {}
for _, node in ipairs(treeNodes) do
    for _, parentId in ipairs(node.parents) do connections[#connections + 1] = {from = parentId, to = node.id} end
end

local BORDER_RADIUS = 6
local CONTENT_NAME = "TowerUpgradesContent"
local LINE_THICKNESS = 3.0
local NODE_GLOW_SIZE = 18.0
local LABEL_FONT = "Lora-VariableFont_wght"
local LABEL_FONT_SIZE = 13.0

local function nodeWidgetName(nodeId) return "TowerUpgradesNode_" .. nodeId end

function TowerUpgradesPanel:new(host, overlay)
    assert(host ~= nil and overlay ~= nil)

    local newObj = {
        host = host,
        isVisible = false,
        background = nil,
        scrollList = nil,
        contentContainer = nil,
        closeButton = nil,
        upgradeButtons = {},
        nodeLabels = {},
        connectionLines = {},
        panelWidth = 0,
        panelHeight = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.background = UiRectangle:new(host, "TowerUpgradesPanelBg")
    overlay:addWidget(newObj.background)

    newObj.scrollList = UiScrollList:new(host, "TowerUpgradesScrollList")
    overlay:addWidget(newObj.scrollList)

    -- One container = single item of the scroll list. Tree nodes, labels and connection lines live inside it. ---
    newObj.contentContainer = UiItem:new(host, CONTENT_NAME)
    overlay:addWidget(newObj.contentContainer)

    newObj.closeButton = ImageButton:new(host, overlay, "TowerUpgradesCloseButton")
    overlay:addCompoundWidget(newObj.closeButton)

    for i = 1, #treeNodes do
        local btn = UpgradeIcon:new(host, nodeWidgetName(treeNodes[i].id))
        overlay:addWidget(btn)
        newObj.upgradeButtons[i] = btn

        local label = UiLabel:new(host, LABEL_FONT, "TowerUpgradesLabel_" .. treeNodes[i].id)
        overlay:addWidget(label)
        newObj.nodeLabels[i] = label
    end

    for i = 1, #connections do
        local line = ConnectionLine:new(host, "TowerUpgradesLine" .. tostring(i))
        overlay:addWidget(line)
        newObj.connectionLines[i] = line
    end

    newObj.closeButton:subscribeOnMouseInputClickedCallback(function()
        newObj:setIsVisible(false)
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
    end)
    newObj.closeButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newObj.closeButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            newObj.closeButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    local function dropGlowVisibleState() for i = 1, #treeNodes do newObj.upgradeButtons[i]:setGlowVisible(false) end end

    for i = 1, #treeNodes do
        local btn = newObj.upgradeButtons[i]
        btn:subscribeOnMouseInputClickedCallback(function()
            dropGlowVisibleState()
            btn:setGlowVisible(true)
        end)
    end

    return newObj
end

function TowerUpgradesPanel:onPreCompoundWidgetInitialize() end

function TowerUpgradesPanel:onCompoundWidgetInitialize() end

function TowerUpgradesPanel:setupLayout(canvasName, windowWidth, windowHeight)
    local panelWidth = math.floor(windowWidth * 0.62)
    local panelHeight = math.floor(windowHeight * 0.85)
    self.panelWidth = panelWidth
    self.panelHeight = panelHeight

    local closeBtnSize = math.floor(panelHeight * 0.06)
    local nodeSize = math.floor(panelHeight * 0.13)
    local scrollTopMargin = closeBtnSize + 4

    self.background:setParent(self.host, canvasName, canvasName)
    self.background:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                              canvasName)
    self.background:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                              canvasName)
    self.background:setWidth(panelWidth)
    self.background:setHeight(panelHeight)
    self.background:setColorHexValue(Styles.Colors.panelColor)
    self.background:setBorderRadius(BORDER_RADIUS)
    self.background:setZOrder(10)
    self.background:setIsVisible(false)

    self.scrollList:setParent(self.host, canvasName, self.background.widgetName)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.background.widgetName,
                              scrollTopMargin)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                              self.background.widgetName, 0)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.background.widgetName, 8)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, self.background.widgetName,
                              8)
    self.scrollList:setSpacing(0)
    self.scrollList:setScrollSpeed(40)
    self.scrollList:setZOrder(11)
    self.scrollList:setIsVisible(false)
    self.scrollList:setScrollbarSide(UiScrollList.ScrollbarSide.NONE)
    self.scrollList:setCanBloomBeApplied(false)

    local contentWidth = panelWidth - 24
    local contentHeight = panelHeight - scrollTopMargin
    self.contentContainer:setParent(self.host, canvasName, self.scrollList.widgetName)
    self.contentContainer:setWidth(contentWidth)
    self.contentContainer:setHeight(contentHeight)
    self.contentContainer:setZOrder(11)
    self.contentContainer:setIsVisible(false)

    self.closeButton:setParent(self.host, canvasName, self.background.widgetName)
    self.closeButton:setWidth(closeBtnSize)
    self.closeButton:setHeight(closeBtnSize)
    self.closeButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.closeButton:setButtonBorderRadius(BORDER_RADIUS)
    self.closeButton:setImageTextureSource("cancel.png")
    self.closeButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.background.widgetName, 2)
    self.closeButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, self.background.widgetName,
                               2)
    self.closeButton:setZOrder(15)
    self.closeButton:setIsVisible(false)

    for i = 1, #treeNodes do
        local node = treeNodes[i]
        local btn = self.upgradeButtons[i]
        btn:setParent(self.host, canvasName, CONTENT_NAME)
        btn:setWidth(nodeSize)
        btn:setHeight(nodeSize)
        btn:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER, CONTENT_NAME)
        btn:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER, CONTENT_NAME)
        btn:setHorizontalCenterOffset(math.floor((node.x / 100.0 - 0.5) * contentWidth))
        btn:setVerticalCenterOffset(math.floor((0.5 - node.y / 100.0) * contentHeight))
        btn:setTextureSource(node.texture)
        btn:setZOrder(13)
        btn:setRotationDegrees(180)
        btn:setBorderColorHexValue(node.color)
        btn:setGlowColorHexValue(node.glow)
        btn:setFillColorHexValue(node.color)
        btn:setGlowSizePx(NODE_GLOW_SIZE)
        btn:setIsVisible(false)
        btn:setCanBloomBeApplied(false)
        btn:enableMouseInputReceiverBase(self.host)
    end

    local labelWidth = math.floor(nodeSize * 3.2)
    local labelHeight = math.floor(nodeSize * 0.42)
    local labelGap = math.floor(nodeSize * 0.1)
    for i = 1, #treeNodes do
        local node = treeNodes[i]
        local label = self.nodeLabels[i]
        label:setParent(self.host, canvasName, CONTENT_NAME)
        label:setWidth(labelWidth)
        label:setHeight(labelHeight)
        label:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                        nodeWidgetName(node.id))
        label:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, nodeWidgetName(node.id), labelGap)
        label:setText(node.name)
        label:setFontSize(LABEL_FONT_SIZE)
        label:setTextColorHexValue(node.glow)
        label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        label:setZOrder(14)
        label:setIsVisible(false)
    end

    for i = 1, #connections do
        local conn = connections[i]
        local line = self.connectionLines[i]
        line:setParent(self.host, canvasName, CONTENT_NAME)
        line:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, CONTENT_NAME)
        line:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, CONTENT_NAME)
        line:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, CONTENT_NAME)
        line:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, CONTENT_NAME)
        line:setStartAnchorTarget(nodeWidgetName(conn.from))
        line:setEndAnchorTarget(nodeWidgetName(conn.to))
        line:setColorHexValue(nodeById[conn.to].glow)
        line:setThicknessPx(LINE_THICKNESS)
        line:setZOrder(12)
        line:setIsVisible(false)
        line:setCanBloomBeApplied(false)
        line:setDashPattern(4, 2)
    end
end

function TowerUpgradesPanel:setIsVisible(isVisible)
    self.isVisible = isVisible
    self.background:setIsVisible(isVisible)
    self.scrollList:setIsVisible(isVisible)
    self.contentContainer:setIsVisible(isVisible)
    self.closeButton:setIsVisible(isVisible)
    for i = 1, #self.upgradeButtons do self.upgradeButtons[i]:setIsVisible(isVisible) end
    for i = 1, #self.nodeLabels do self.nodeLabels[i]:setIsVisible(isVisible) end
    for i = 1, #self.connectionLines do self.connectionLines[i]:setIsVisible(isVisible) end
end

function TowerUpgradesPanel:update() end

return TowerUpgradesPanel
