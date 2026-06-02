--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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
local UiItem = require("Ui/Core/uiItem")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local UiImage = require("Ui/Core/uiImage")
local LabelButton = require("Ui/Widgets/LabelButton")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")

local FONT = "JetBrainsMono-VariableFont_wght"

-- Tower context panel (mockup CbTowerContext, right shell): header with the tower name + icon, a stats
-- block and the ДЕМОНТАЖ / ПРОКАЧАТЬ actions. The numeric stats have no engine data yet → placeholders.
local STAT_KEYS = {"УРОН/С", "ДАЛЬНОСТЬ", "ТЕМП", "ЭФФЕКТ"}

SelectedTowerPanel = {}

function SelectedTowerPanel:new(host, overlay, widgetName, config)
    config = config or {}
    local newObj = {
        host = host,
        widgetName = widgetName or "SelectedTowerPanel",
        onDemolishClicked = config.onDemolishClicked,
        onUpgradeClicked = config.onUpgradeClicked,
        mainContainer = nil,
        background = nil,
        weaponIcon = nil,
        titleLabel = nil,
        subtitleLabel = nil,
        statKeyLabels = {},
        statValueLabels = {},
        demolishButton = nil,
        upgradeButton = nil,
        toggleWidgets = {},
        width = 0,
        height = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.mainContainer = UiItem:new(host, newObj.widgetName)
    overlay:addWidget(newObj.mainContainer)

    newObj.background = UiRectangle:new(host, newObj.widgetName .. "_Bg")
    overlay:addWidget(newObj.background)

    newObj.weaponIcon = UiImage:new(host, newObj.widgetName .. "_Icon")
    overlay:addWidget(newObj.weaponIcon)

    newObj.titleLabel = UiLabel:new(host, FONT, newObj.widgetName .. "_Title")
    overlay:addWidget(newObj.titleLabel)

    newObj.subtitleLabel = UiLabel:new(host, FONT, newObj.widgetName .. "_Subtitle")
    overlay:addWidget(newObj.subtitleLabel)

    for i = 1, #STAT_KEYS do
        newObj.statKeyLabels[i] = UiLabel:new(host, FONT, newObj.widgetName .. "_StatKey" .. tostring(i))
        overlay:addWidget(newObj.statKeyLabels[i])
        newObj.statValueLabels[i] = UiLabel:new(host, FONT, newObj.widgetName .. "_StatValue" .. tostring(i))
        overlay:addWidget(newObj.statValueLabels[i])
    end

    newObj.demolishButton = LabelButton:new(host, overlay, FONT, newObj.widgetName .. "_Demolish")
    overlay:addCompoundWidget(newObj.demolishButton)

    newObj.upgradeButton = LabelButton:new(host, overlay, FONT, newObj.widgetName .. "_Upgrade")
    overlay:addCompoundWidget(newObj.upgradeButton)

    -- Every leaf widget is toggled explicitly in setIsVisible (rather than relying on parent-visibility
    -- culling) to avoid the layout-solve glitch that hiding an anchored container can introduce.
    newObj.toggleWidgets = {
        newObj.mainContainer, newObj.background, newObj.weaponIcon, newObj.titleLabel, newObj.subtitleLabel,
        newObj.demolishButton, newObj.upgradeButton
    }
    for i = 1, #STAT_KEYS do
        newObj.toggleWidgets[#newObj.toggleWidgets + 1] = newObj.statKeyLabels[i]
        newObj.toggleWidgets[#newObj.toggleWidgets + 1] = newObj.statValueLabels[i]
    end

    newObj.demolishButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        newObj.demolishButton:setButtonColorHexValue(
            newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Styles.Combat.chipHoverColor or
                Styles.Combat.chipColor)
    end)
    newObj.demolishButton:subscribeOnMouseInputClickedCallback(function()
        if newObj.onDemolishClicked then newObj.onDemolishClicked() end
    end)

    newObj.upgradeButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        newObj.upgradeButton:setButtonColorHexValue(
            newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Styles.Combat.cyanGlow or
                Styles.Combat.cyan)
    end)
    newObj.upgradeButton:subscribeOnMouseInputClickedCallback(function()
        if newObj.onUpgradeClicked then newObj.onUpgradeClicked() end
    end)

    return newObj
end

function SelectedTowerPanel:subscribeOnLuaProxiesReady(callback) self.luaProxiesReadyCallback = callback end

-- The inner LabelButtons register themselves as compound widgets on the overlay, so the overlay drives
-- their init directly; the panel itself has nothing extra to do here.
function SelectedTowerPanel:onPreCompoundWidgetInitialize() end
function SelectedTowerPanel:onCompoundWidgetInitialize() end

function SelectedTowerPanel:update(host, deltaTimeSec) end

function SelectedTowerPanel:setParent(host, overlayCanvasName, parentName)
    self.overlayCanvasName = overlayCanvasName
    self.mainContainer:setParent(host, overlayCanvasName, parentName)
end

function SelectedTowerPanel:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self.mainContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
end

function SelectedTowerPanel:setWidth(width)
    self.width = width
    self.mainContainer:setWidth(width)
end

function SelectedTowerPanel:setHeight(height)
    self.height = height
    self.mainContainer:setHeight(height)
end

function SelectedTowerPanel:setupLayout()
    local host = self.host
    local canvasName = self.overlayCanvasName
    local A = UiItemBase.UiAnchorType
    local HALIGN = UiLabel.TextHorizontalAlignmentType
    local VALIGN = UiLabel.TextVerticalAlignmentType
    local Combat = Styles.Combat
    local panelName = self.mainContainer.widgetName

    self.background:setParent(host, canvasName, panelName)
    self.background:fill(panelName)
    self.background:setColorHexValue(Combat.panelColor)
    self.background:setOpacity(Combat.panelOpacity)
    self.background:setBorderRadius(Combat.panelBorderRadius)

    local bgName = self.background.widgetName

    -- Header: icon + title + subtitle
    self.weaponIcon:setParent(host, canvasName, bgName)
    self.weaponIcon:setAnchor(A.LEFT, A.LEFT, bgName, 16)
    self.weaponIcon:setAnchor(A.TOP, A.TOP, bgName, 16)
    self.weaponIcon:setWidth(44)
    self.weaponIcon:setHeight(44)
    self.weaponIcon:setTextureSource("space_station_img.png")

    self.titleLabel:setParent(host, canvasName, bgName)
    self.titleLabel:setAnchor(A.LEFT, A.RIGHT, self.weaponIcon.widgetName, 12)
    self.titleLabel:setAnchor(A.TOP, A.TOP, bgName, 18)
    self.titleLabel:setWidth(self.width - 90)
    self.titleLabel:setHeight(20)
    self.titleLabel:setFontSize(15)
    self.titleLabel:setTextColorHexValue(Combat.cyanGlow)
    self.titleLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.titleLabel:setText("БАШНЯ")

    self.subtitleLabel:setParent(host, canvasName, bgName)
    self.subtitleLabel:setAnchor(A.LEFT, A.RIGHT, self.weaponIcon.widgetName, 12)
    self.subtitleLabel:setAnchor(A.TOP, A.TOP, bgName, 40)
    self.subtitleLabel:setWidth(self.width - 90)
    self.subtitleLabel:setHeight(16)
    self.subtitleLabel:setFontSize(10)
    self.subtitleLabel:setTextColorHexValue(Combat.textVeryDim)
    self.subtitleLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.subtitleLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.subtitleLabel:setText("ИНД-0042 · СЕКТОР C-4") -- TODO: real object id / sector

    -- Stats block (placeholders) — TODO: real per-tower stats from the engine
    local statTop = 78
    local rowHeight = 30
    for i = 1, #STAT_KEYS do
        local rowY = statTop + (i - 1) * rowHeight
        local keyLabel = self.statKeyLabels[i]
        keyLabel:setParent(host, canvasName, bgName)
        keyLabel:setAnchor(A.LEFT, A.LEFT, bgName, 18)
        keyLabel:setAnchor(A.TOP, A.TOP, bgName, rowY)
        keyLabel:setWidth(self.width / 2)
        keyLabel:setHeight(20)
        keyLabel:setFontSize(12)
        keyLabel:setTextColorHexValue(Combat.textDim)
        keyLabel:setTextHorizontalAlignment(HALIGN.LEFT)
        keyLabel:setTextVerticalAlignment(VALIGN.CENTER)
        keyLabel:setText(STAT_KEYS[i])

        local valueLabel = self.statValueLabels[i]
        valueLabel:setParent(host, canvasName, bgName)
        valueLabel:setAnchor(A.RIGHT, A.RIGHT, bgName, 18)
        valueLabel:setAnchor(A.TOP, A.TOP, bgName, rowY)
        valueLabel:setWidth(self.width / 2 - 30)
        valueLabel:setHeight(20)
        valueLabel:setFontSize(12)
        valueLabel:setTextColorHexValue(Combat.textBright)
        valueLabel:setTextHorizontalAlignment(HALIGN.RIGHT)
        valueLabel:setTextVerticalAlignment(VALIGN.CENTER)
        valueLabel:setText("—")
    end

    -- Actions: ДЕМОНТАЖ (relocated from the palette) + ПРОКАЧАТЬ
    local actionWidth = (self.width - 16 * 2 - 10) / 2
    self.demolishButton:setParent(host, canvasName, bgName)
    self.demolishButton:setWidth(actionWidth)
    self.demolishButton:setHeight(40)
    self.demolishButton:setAnchor(A.LEFT, A.LEFT, bgName, 16)
    self.demolishButton:setAnchor(A.BOTTOM, A.BOTTOM, bgName, 16)
    self.demolishButton:setButtonColorHexValue(Combat.chipColor)
    self.demolishButton:setButtonBorderRadius(Combat.chipBorderRadius)
    self.demolishButton:setLabelText("ДЕМОНТАЖ")
    self.demolishButton:setLabelFontSize(12)
    self.demolishButton:setLabelTextColorHexValue(Combat.danger)
    self.demolishButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
    self.demolishButton:setLabelTextVerticalAlignment(VALIGN.CENTER)

    self.upgradeButton:setParent(host, canvasName, bgName)
    self.upgradeButton:setWidth(actionWidth)
    self.upgradeButton:setHeight(40)
    self.upgradeButton:setAnchor(A.RIGHT, A.RIGHT, bgName, 16)
    self.upgradeButton:setAnchor(A.BOTTOM, A.BOTTOM, bgName, 16)
    self.upgradeButton:setButtonColorHexValue(Combat.cyan)
    self.upgradeButton:setButtonBorderRadius(Combat.chipBorderRadius)
    self.upgradeButton:setLabelText("ПРОКАЧАТЬ")
    self.upgradeButton:setLabelFontSize(12)
    self.upgradeButton:setLabelTextColorHexValue(Combat.textBright)
    self.upgradeButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
    self.upgradeButton:setLabelTextVerticalAlignment(VALIGN.CENTER)
end

function SelectedTowerPanel:setIsVisible(isVisible)
    for _, widget in ipairs(self.toggleWidgets) do widget:setIsVisible(isVisible) end
end

function SelectedTowerPanel:setWeaponImage(imageName) self.weaponIcon:setTextureSource(imageName) end

function SelectedTowerPanel:setTowerTitle(title) self.titleLabel:setText(title) end

return SelectedTowerPanel
