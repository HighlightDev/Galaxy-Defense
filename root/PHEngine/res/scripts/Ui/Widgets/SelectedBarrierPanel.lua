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

-- Barrier context panel — a trimmed-down variant of SelectedTowerPanel. A barrier has no upgrade path
-- and no per-tower stats, so the panel keeps only the header (icon + title) and the ДЕМОНТАЖ action.
SelectedBarrierPanel = {}

function SelectedBarrierPanel:new(host, overlay, widgetName, config)
    config = config or {}
    local newObj = {
        host = host,
        widgetName = widgetName or "SelectedBarrierPanel",
        onDemolishClicked = config.onDemolishClicked,
        mainContainer = nil,
        background = nil,
        icon = nil,
        titleLabel = nil,
        subtitleLabel = nil,
        demolishButton = nil,
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

    newObj.icon = UiImage:new(host, newObj.widgetName .. "_Icon")
    overlay:addWidget(newObj.icon)

    newObj.titleLabel = UiLabel:new(host, FONT, newObj.widgetName .. "_Title")
    overlay:addWidget(newObj.titleLabel)

    newObj.subtitleLabel = UiLabel:new(host, FONT, newObj.widgetName .. "_Subtitle")
    overlay:addWidget(newObj.subtitleLabel)

    newObj.demolishButton = LabelButton:new(host, overlay, FONT, newObj.widgetName .. "_Demolish")
    overlay:addCompoundWidget(newObj.demolishButton)

    -- Every leaf widget is toggled explicitly in setIsVisible (rather than relying on parent-visibility
    -- culling) to avoid the layout-solve glitch that hiding an anchored container can introduce.
    newObj.toggleWidgets = {
        newObj.mainContainer, newObj.background, newObj.icon, newObj.titleLabel, newObj.subtitleLabel,
        newObj.demolishButton
    }

    newObj.demolishButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        newObj.demolishButton:setButtonColorHexValue(newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and
                                                         Styles.Combat.chipHoverColor or Styles.Combat.chipColor)
    end)
    newObj.demolishButton:subscribeOnMouseInputClickedCallback(function()
        if newObj.onDemolishClicked then newObj.onDemolishClicked() end
    end)

    return newObj
end

function SelectedBarrierPanel:subscribeOnLuaProxiesReady(callback) self.luaProxiesReadyCallback = callback end

-- The inner LabelButton registers itself as a compound widget on the overlay, so the overlay drives
-- its init directly; the panel itself has nothing extra to do here.
function SelectedBarrierPanel:onPreCompoundWidgetInitialize() end
function SelectedBarrierPanel:onCompoundWidgetInitialize() end

function SelectedBarrierPanel:update(host, deltaTimeSec) end

function SelectedBarrierPanel:setParent(host, overlayCanvasName, parentName)
    self.overlayCanvasName = overlayCanvasName
    self.mainContainer:setParent(host, overlayCanvasName, parentName)
end

function SelectedBarrierPanel:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self.mainContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
end

function SelectedBarrierPanel:setWidth(width)
    self.width = width
    self.mainContainer:setWidth(width)
end

function SelectedBarrierPanel:setHeight(height)
    self.height = height
    self.mainContainer:setHeight(height)
end

function SelectedBarrierPanel:setupLayout()
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
    self.icon:setParent(host, canvasName, bgName)
    self.icon:setAnchor(A.LEFT, A.LEFT, bgName, 16)
    self.icon:setAnchor(A.TOP, A.TOP, bgName, 16)
    self.icon:setWidth(44)
    self.icon:setHeight(44)
    self.icon:setTextureSource("space_station_img.png") -- TODO: dedicated barrier icon when art is available

    self.titleLabel:setParent(host, canvasName, bgName)
    self.titleLabel:setAnchor(A.LEFT, A.RIGHT, self.icon.widgetName, 12)
    self.titleLabel:setAnchor(A.TOP, A.TOP, bgName, 18)
    self.titleLabel:setWidth(self.width - 90)
    self.titleLabel:setHeight(20)
    self.titleLabel:setFontSize(15)
    self.titleLabel:setTextColorHexValue(Combat.cyanGlow)
    self.titleLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.titleLabel:setText("БАРЬЕР")

    self.subtitleLabel:setParent(host, canvasName, bgName)
    self.subtitleLabel:setAnchor(A.LEFT, A.RIGHT, self.icon.widgetName, 12)
    self.subtitleLabel:setAnchor(A.TOP, A.TOP, bgName, 40)
    self.subtitleLabel:setWidth(self.width - 90)
    self.subtitleLabel:setHeight(16)
    self.subtitleLabel:setFontSize(10)
    self.subtitleLabel:setTextColorHexValue(Combat.textVeryDim)
    self.subtitleLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.subtitleLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.subtitleLabel:setText("ЭНЕРГОЩИТ") -- TODO: real object id / sector

    -- Action: ДЕМОНТАЖ only (a barrier has no upgrade path) — spans the full panel width.
    self.demolishButton:setParent(host, canvasName, bgName)
    self.demolishButton:setWidth(self.width - 16 * 2)
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
end

function SelectedBarrierPanel:setIsVisible(isVisible)
    for _, widget in ipairs(self.toggleWidgets) do widget:setIsVisible(isVisible) end
end

function SelectedBarrierPanel:setBarrierImage(imageName) self.icon:setTextureSource(imageName) end

function SelectedBarrierPanel:setBarrierTitle(title) self.titleLabel:setText(title) end

return SelectedBarrierPanel
