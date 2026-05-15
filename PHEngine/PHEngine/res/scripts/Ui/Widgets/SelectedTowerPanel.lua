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
local UiItem = require("Ui/Core/uiItem")
local ImageAndLabelTile = require("Ui/Widgets/ImageAndLabelTile")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")

SelectedTowerPanel = {}

function SelectedTowerPanel:new(host, overlay, widgetName)
    local newObj = {
        host = host,
        widgetName = widgetName or "SelectedTowerPanel",
        mainContainer = nil,
        selectedTowerTile = nil,
        luaProxiesReadyCallback = nil,
        isLuaProxyReady = false,
        allWidgetLuaProxiesReady = false,
        width = 0,
        height = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.mainContainer = UiItem:new(host, newObj.widgetName)
    overlay:addWidget(newObj.mainContainer)

    newObj.selectedTowerTile = ImageAndLabelTile:new(host, overlay, newObj.widgetName .. "_Tile")

    local function checkAllWidgetLuaProxiesAreReady()
        if newObj.mainContainer.luaProxyReady == true and newObj.selectedTowerTile.allWidgetLuaProxiesReady == true then
            newObj.allWidgetLuaProxiesReady = true
            if newObj.luaProxiesReadyCallback ~= nil then
                newObj.luaProxiesReadyCallback(newObj.host)
            end
        end
    end

    newObj.mainContainer:subscribeOnLuaProxyReady(checkAllWidgetLuaProxiesAreReady)
    newObj.selectedTowerTile:subscribeOnLuaProxiesReady(checkAllWidgetLuaProxiesAreReady)

    return newObj
end

function SelectedTowerPanel:subscribeOnLuaProxiesReady(callback)
    if self.allWidgetLuaProxiesReady == true then
        callback(self.host)
    else
        self.luaProxiesReadyCallback = callback
    end
end

function SelectedTowerPanel:onPreCompoundWidgetInitialize()
    self.selectedTowerTile:onPreCompoundWidgetInitialize()
end

function SelectedTowerPanel:onCompoundWidgetInitialize()
    self.selectedTowerTile:onCompoundWidgetInitialize()
end

function SelectedTowerPanel:update(host, deltaTimeSec)
end

function SelectedTowerPanel:setParent(host, overlayCanvasName, parentName)
    self.mainContainer:setParent(host, overlayCanvasName, parentName)
    self.selectedTowerTile:setParent(host, overlayCanvasName, self.mainContainer.widgetName)
end

function SelectedTowerPanel:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self.mainContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
end

function SelectedTowerPanel:setWidth(width)
    self.width = width
    self.mainContainer:setWidth(width)
    self.selectedTowerTile:setWidth(width)
end

function SelectedTowerPanel:setHeight(height)
    self.height = height
    self.mainContainer:setHeight(height)
    self.selectedTowerTile:setHeight(height)
end

function SelectedTowerPanel:setupLayout()
    self.selectedTowerTile:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.mainContainer.widgetName, 0)
    self.selectedTowerTile:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.mainContainer.widgetName, 0)
    self.selectedTowerTile:setTextureSource("diamond.png")
    self.selectedTowerTile:setLabelVisibility(false)
    self.selectedTowerTile:setBackgroundTileColorHexValue(Styles.Colors.panelColor)
    self.selectedTowerTile:setRotationDegrees(180)
    self.selectedTowerTile:setIsFlipped(true)
end

function SelectedTowerPanel:setIsVisible(isVisible)
    self.mainContainer:setIsVisible(isVisible)
    self.selectedTowerTile:setIsVisible(isVisible)
end

function SelectedTowerPanel:setWeaponImage(imageName)
    self.selectedTowerTile:setTextureSource(imageName)
end

return SelectedTowerPanel