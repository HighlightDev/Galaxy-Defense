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
local ImageButton = require("Ui/Widgets/ImageButton")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")
local EventsHelper = require("Ui/Core/eventsHelper")
local UpgradeIcon = require("Ui/Core/uiUpgradeIcon")

TowerUpgradesPanel = {}

local iconsCfg = {
    {texture = "cryo_cannon.png", borderColor = 0x23208d, glowColor = 0x23208d},
    {texture = "electric_beam.png", borderColor = 0x88008d, glowColor = 0x88008d},
    {texture = "force_barrier.png", borderColor = 0xcf910e, glowColor = 0xcf910e},
    {texture = "gravity_bomb.png", borderColor = 0x8700bb, glowColor = 0x8700bb},
    {texture = "he_rocket.png", borderColor = 0xf24d00, glowColor = 0xf24d00},
    {texture = "ice_beam.png", borderColor = 0x0c6a83, glowColor = 0x0c6a83},
    {texture = "ice_rocket.png", borderColor = 0x1dcbe7, glowColor = 0x1dcbe7},
    {texture = "nano_beam.png", borderColor = 0x00c77f, glowColor = 0x00c77f},
    {texture = "plasma_rocket.png", borderColor = 0x750653, glowColor = 0x750653},
    {texture = "quantum_nexus.png", borderColor = 0x4e3f88, glowColor = 0x4e3f88},
    {texture = "repair_beam.png", borderColor = 0x209a5d, glowColor = 0x209a5d}
}

local BORDER_RADIUS = 6

function TowerUpgradesPanel:new(host, overlay)
    assert(host ~= nil and overlay ~= nil)

    local newObj = {
        host = host,
        isVisible = false,
        background = nil,
        scrollList = nil,
        closeButton = nil,
        upgradeButtons = {},
        panelWidth = 0,
        panelHeight = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.background = UiRectangle:new(host, "TowerUpgradesPanelBg")
    overlay:addWidget(newObj.background)

    newObj.scrollList = UiScrollList:new(host, "TowerUpgradesScrollList")
    overlay:addWidget(newObj.scrollList)

    newObj.closeButton = ImageButton:new(host, overlay, "TowerUpgradesCloseButton")
    overlay:addCompoundWidget(newObj.closeButton)

    for i = 1, #iconsCfg do
        local btn = UpgradeIcon:new(host, "TowerUpgradesUpgradeButton" .. tostring(i))
        overlay:addWidget(btn)
        newObj.upgradeButtons[i] = btn
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

    local dummyLabels = {"Speed Up", "Range Up", "Damage Up"}
    for i = 1, #iconsCfg do
        local btn = newObj.upgradeButtons[i]
        btn:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                btn:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                btn:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end)
        btn:subscribeOnMouseInputClickedCallback(function()
            print("TowerUpgradesPanel: dummy perk clicked: " .. dummyLabels[((i - 1) % #dummyLabels) + 1])
        end)
    end

    return newObj
end

function TowerUpgradesPanel:onPreCompoundWidgetInitialize() end

function TowerUpgradesPanel:onCompoundWidgetInitialize() end

function TowerUpgradesPanel:setupLayout(canvasName, windowWidth, windowHeight)
    local panelWidth = math.floor(windowWidth * 0.5)
    local panelHeight = math.floor(windowHeight * 0.5)
    self.panelWidth = panelWidth
    self.panelHeight = panelHeight

    local closeBtnSize = math.floor(panelHeight * 0.08)
    local itemHeight = math.floor(panelHeight * 0.18)
    local itemSpacing = math.floor(panelHeight * 0.03)

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

    local scrollTopMargin = closeBtnSize + 4
    self.scrollList:setParent(self.host, canvasName, self.background.widgetName)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.background.widgetName,
                              scrollTopMargin)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                              self.background.widgetName, 0)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.background.widgetName, 8)
    self.scrollList:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, self.background.widgetName,
                              8)
    self.scrollList:setSpacing(itemSpacing)
    self.scrollList:setScrollSpeed(40)
    self.scrollList:setZOrder(11)
    self.scrollList:setIsVisible(false)
    self.scrollList:setScrollbarSide(UiScrollList.ScrollbarSide.RIGHT)
    self.scrollList:setScrollbarBackgroundColorHexValue(Styles.Colors.panelColor)
    self.scrollList:setScrollbarThumbColorHexValue(Styles.Colors.buttonColor)
    self.scrollList:setScrollbarThicknessPixels(6)
    self.scrollList:setCanBloomBeApplied(false)

    self.closeButton:setParent(self.host, canvasName, self.background.widgetName)
    self.closeButton:setWidth(closeBtnSize)
    self.closeButton:setHeight(closeBtnSize)
    self.closeButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    self.closeButton:setButtonBorderRadius(BORDER_RADIUS)
    self.closeButton:setImageTextureSource("cancel.png")
    self.closeButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.background.widgetName, 2)
    self.closeButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, self.background.widgetName,
                               2)
    self.closeButton:setZOrder(12)
    self.closeButton:setIsVisible(false)

    for i = 1, #iconsCfg do
        local btn = self.upgradeButtons[i]
        btn:setParent(self.host, canvasName, self.scrollList.widgetName)
        btn:setWidth(itemHeight)
        btn:setHeight(itemHeight)
        btn:setTextureSource(iconsCfg[i].texture)
        btn:setZOrder(12)
        btn:setRotationDegrees(180)
        btn:setBorderColorHexValue(iconsCfg[i].borderColor)
        btn:setGlowColorHexValue(iconsCfg[i].glowColor)
        btn:setIsVisible(false)
        btn:setCanBloomBeApplied(true)
    end
end

function TowerUpgradesPanel:setIsVisible(isVisible)
    print("TowerUpgradesPanel:setIsVisible called, isVisible=" .. tostring(isVisible) .. ", setupDone=" ..
              tostring(self.panelWidth > 0))
    self.isVisible = isVisible
    self.background:setIsVisible(isVisible)
    self.scrollList:setIsVisible(isVisible)
    self.closeButton:setIsVisible(isVisible)
    for i = 1, #self.upgradeButtons do self.upgradeButtons[i]:setIsVisible(isVisible) end
end

function TowerUpgradesPanel:update() end

return TowerUpgradesPanel
