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
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")

-- Operation objectives panel, modelled after the mockup's CbObjectives (.cb-obj): a bordered card with
-- a header and a vertical list of objective rows (status icon + label + value). Each row mirrors
-- .cb-obj__item — a dark plate, a tinted status glyph, the objective text and a right-aligned counter.
-- The combat HUD feeds it a plain item list built from the engine's requirement trackers.
local FONT = "JetBrainsMono-VariableFont_wght"

local ITEM_POOL = 5
local ITEM_HEIGHT = 42
local ITEM_GAP = 6
local PAD = 16
local HEADER_FONT = 11
local ITEM_FONT = 12

local COLOR_ITEM_BG = 0x070d18
local COLOR_HEADER_RULE = 0x16314f
local COLOR_DONE = 0x86efac

ObjectivesPanel = {}

function ObjectivesPanel:new(host, overlay, widgetName)
    local newObj = {
        host = host,
        widgetName = widgetName or "ObjectivesPanel",
        overlayCanvasName = nil,
        mainContainer = nil,
        background = nil,
        headerLabel = nil,
        headerRule = nil,
        items = {},
        width = 0,
        height = 0,
        itemsTop = 0,
        visibleCount = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.mainContainer = UiItem:new(host, newObj.widgetName)
    overlay:addWidget(newObj.mainContainer)

    newObj.background = UiRectangle:new(host, newObj.widgetName .. "_Bg")
    overlay:addWidget(newObj.background)

    newObj.headerLabel = UiLabel:new(host, FONT, newObj.widgetName .. "_Hdr")
    overlay:addWidget(newObj.headerLabel)

    newObj.headerRule = UiRectangle:new(host, newObj.widgetName .. "_Rule")
    overlay:addWidget(newObj.headerRule)

    for i = 1, ITEM_POOL do
        local item = {
            bg = UiRectangle:new(host, newObj.widgetName .. "_Item" .. i .. "_Bg"),
            icon = UiImage:new(host, newObj.widgetName .. "_Item" .. i .. "_Ico"),
            label = UiLabel:new(host, FONT, newObj.widgetName .. "_Item" .. i .. "_Lbl"),
            value = UiLabel:new(host, FONT, newObj.widgetName .. "_Item" .. i .. "_Val")
        }
        overlay:addWidget(item.bg)
        overlay:addWidget(item.icon)
        overlay:addWidget(item.label)
        overlay:addWidget(item.value)
        newObj.items[i] = item
    end

    return newObj
end

-- Compound-widget hooks (the panel owns no nested compound widgets; nothing extra to init).
function ObjectivesPanel:subscribeOnLuaProxiesReady(callback) self.luaProxiesReadyCallback = callback end
function ObjectivesPanel:onPreCompoundWidgetInitialize() end
function ObjectivesPanel:onCompoundWidgetInitialize() end
function ObjectivesPanel:update(host, deltaTimeSec) end

function ObjectivesPanel:setParent(host, overlayCanvasName, parentName)
    self.overlayCanvasName = overlayCanvasName
    self.mainContainer:setParent(host, overlayCanvasName, parentName)
end

function ObjectivesPanel:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self.mainContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
end

function ObjectivesPanel:setWidth(width)
    self.width = width
    self.mainContainer:setWidth(width)
end

function ObjectivesPanel:setHeight(height)
    self.height = height
    self.mainContainer:setHeight(height)
end

function ObjectivesPanel:setupLayout()
    local host = self.host
    local canvasName = self.overlayCanvasName
    local A = UiItemBase.UiAnchorType
    local HALIGN = UiLabel.TextHorizontalAlignmentType
    local VALIGN = UiLabel.TextVerticalAlignmentType
    local C = Styles.Combat
    local function lh(fontSize) return UiTextSizing.safeLineHeight(fontSize, FONT) end
    local panelName = self.mainContainer.widgetName

    -- The pooled item plates are parented to the always-visible mainContainer (a non-drawing layout
    -- node) rather than to the background. Toggling the panel hides/shows the background, and the C++
    -- SetChildrenIsVisible would otherwise force the *hidden* pool plates back on (empty rows under the
    -- panel). Keeping them off the background's child list avoids that; their visibility is owned solely
    -- by setItems. Explicit z-orders keep the content above the background surface.
    self.background:setParent(host, canvasName, panelName)
    self.background:fill(panelName)
    self.background:setColorHexValue(C.panelColor)
    self.background:setOpacity(C.panelOpacity)
    self.background:setBorderRadius(C.panelBorderRadius)
    self.background:setZOrder(0)

    self.headerLabel:setParent(host, canvasName, panelName)
    self.headerLabel:setAnchor(A.LEFT, A.LEFT, panelName, PAD)
    self.headerLabel:setAnchor(A.TOP, A.TOP, panelName, 14)
    self.headerLabel:setWidth(self.width - 2 * PAD)
    self.headerLabel:setHeight(lh(HEADER_FONT))
    self.headerLabel:setFontSize(HEADER_FONT)
    self.headerLabel:setTextColorHexValue(C.cyanGlow)
    self.headerLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.headerLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.headerLabel:setText("[ ЗАДАЧИ ОПЕРАЦИИ ]")
    self.headerLabel:setZOrder(1)

    local ruleY = 14 + lh(HEADER_FONT) + 8
    self.headerRule:setParent(host, canvasName, panelName)
    self.headerRule:setAnchor(A.LEFT, A.LEFT, panelName, PAD)
    self.headerRule:setAnchor(A.RIGHT, A.RIGHT, panelName, PAD)
    self.headerRule:setAnchor(A.TOP, A.TOP, panelName, ruleY)
    self.headerRule:setHeight(1)
    self.headerRule:setColorHexValue(COLOR_HEADER_RULE)
    self.headerRule:setZOrder(1)

    local itemsTop = ruleY + 10
    self.itemsTop = itemsTop
    for i = 1, ITEM_POOL do
        local item = self.items[i]
        local y = itemsTop + (i - 1) * (ITEM_HEIGHT + ITEM_GAP)

        item.bg:setParent(host, canvasName, panelName)
        item.bg:setAnchor(A.LEFT, A.LEFT, panelName, PAD)
        item.bg:setAnchor(A.RIGHT, A.RIGHT, panelName, PAD)
        item.bg:setAnchor(A.TOP, A.TOP, panelName, y)
        item.bg:setHeight(ITEM_HEIGHT)
        item.bg:setColorHexValue(COLOR_ITEM_BG)
        item.bg:setBorderRadius(4)
        item.bg:setZOrder(1)
        item.bg:setIsVisible(false)
        local itemBg = item.bg.widgetName

        item.icon:setParent(host, canvasName, itemBg)
        item.icon:setAnchor(A.LEFT, A.LEFT, itemBg, 11)
        item.icon:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, itemBg)
        item.icon:setWidth(18)
        item.icon:setHeight(18)
        item.icon:setUseImageCustomColor(true)
        item.icon:setIsVisible(false)

        item.label:setParent(host, canvasName, itemBg)
        item.label:setAnchor(A.LEFT, A.LEFT, itemBg, 38)
        item.label:setAnchor(A.RIGHT, A.RIGHT, itemBg, 56)
        item.label:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, itemBg)
        item.label:setHeight(lh(ITEM_FONT))
        item.label:setFontSize(ITEM_FONT)
        item.label:setTextColorHexValue(C.textBright)
        item.label:setTextHorizontalAlignment(HALIGN.LEFT)
        item.label:setTextVerticalAlignment(VALIGN.CENTER)
        item.label:setIsVisible(false)

        item.value:setParent(host, canvasName, itemBg)
        item.value:setAnchor(A.RIGHT, A.RIGHT, itemBg, 11)
        item.value:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, itemBg)
        item.value:setWidth(40)
        item.value:setHeight(lh(ITEM_FONT))
        item.value:setFontSize(ITEM_FONT)
        item.value:setTextColorHexValue(C.cyanGlow)
        item.value:setTextHorizontalAlignment(HALIGN.RIGHT)
        item.value:setTextVerticalAlignment(VALIGN.CENTER)
        item.value:setIsVisible(false)
    end
end

local function hideItem(item)
    item.bg:setIsVisible(false)
    item.icon:setIsVisible(false)
    item.label:setIsVisible(false)
    item.value:setIsVisible(false)
end

-- items: array of { icon = "skull.png", label = "...", value = "12" | nil, color = 0x..., done = bool }
function ObjectivesPanel:setItems(items)
    local C = Styles.Combat
    local count = math.min(#items, ITEM_POOL)
    self.visibleCount = count
    for i = 1, ITEM_POOL do
        local item = self.items[i]
        local data = items[i]
        if i <= count and data ~= nil then
            item.bg:setIsVisible(true)
            item.icon:setIsVisible(true)
            item.icon:setTextureSource(data.icon)
            item.icon:setColorHexValue(data.color or C.cyan)
            item.label:setIsVisible(true)
            item.label:setText(data.label or "")
            item.label:setTextColorHexValue(data.done and C.textDim or C.textBright)
            if data.value ~= nil and data.value ~= "" then
                item.value:setIsVisible(true)
                item.value:setText(tostring(data.value))
                item.value:setTextColorHexValue(data.color or C.cyanGlow)
            else
                item.value:setIsVisible(false)
            end
        else
            hideItem(item)
        end
    end

    -- Size the card to the number of visible rows (the mockup card hugs its content). Only the layout
    -- container is resized; the background fill()s it, so it follows automatically.
    if self.itemsTop > 0 then
        local rows = math.max(count, 1)
        local panelHeight = self.itemsTop + rows * (ITEM_HEIGHT + ITEM_GAP) - ITEM_GAP + PAD
        self.mainContainer:setHeight(panelHeight)
        self.height = panelHeight
    end
end

-- Flash every currently shown objective as completed (stage cleared).
function ObjectivesPanel:markAllDone()
    local C = Styles.Combat
    for i = 1, self.visibleCount do
        local item = self.items[i]
        item.icon:setTextureSource("check.png")
        item.icon:setColorHexValue(COLOR_DONE)
        item.label:setTextColorHexValue(C.textDim)
        item.value:setIsVisible(false)
    end
end

function ObjectivesPanel:setIsVisible(isVisible)
    -- mainContainer is a non-drawing layout node kept always-visible: widgets anchored to it (the tower
    -- panel stacked below) keep a valid rect even while the objectives are hidden. Only the drawing
    -- children are toggled — hiding the layout node would stale its rect (see CombatHudOverlay notes).
    self.background:setIsVisible(isVisible)
    self.headerLabel:setIsVisible(isVisible)
    self.headerRule:setIsVisible(isVisible)
    if not isVisible then for i = 1, ITEM_POOL do hideItem(self.items[i]) end end
end

return ObjectivesPanel
