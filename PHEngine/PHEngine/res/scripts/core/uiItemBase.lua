--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local UiBaseWidget = require("uiBaseWidget")
local json = require("3rdparty/json")

UiItemBase = UiBaseWidget:new()

function UiItemBase:new()
    print("UiItemBase::ctor")

    local uiItemBaseProperties = {
        visible = {
            value = false,
            dirty = false
        },
        z_order = {
            value = 0,
            dirty = false
        },
        width = {
            value = 0,
            dirty = false
        },
        height = {
            value = 0,
            dirty = false
        },
        horizontalCenterOffset = {
            value = 0,
            dirty = false
        },
        verticalCenterOffset = {
            value = 0,
            dirty = false
        },
        anchors = {

        }
    }

    local uiItemBaseObj = UiItemBase.parentClass.new(self)
    uiItemBaseObj.uiItemBaseClass = self
    uiItemBaseObj.uiWidgetName = ""
    uiItemBaseObj.properties = uiItemBaseProperties

    return uiItemBaseObj
end

function UiItemBase:extractUiItemBaseReplicatorData(parsedJsonData)
    if self.uiWidgetName == "" and parsedJsonData["name"] ~= nil then
        self.uiWidgetName = parsedJsonData["name"]
        print("UiItemBase:extractUiItemBaseReplicatorData => name: " .. tostring(self.uiWidgetName))
    end
    if parsedJsonData["visible"] ~= nil then
        print("UiItemBase:extractUiItemBaseReplicatorData => visible")
        self.properties.visible.value = parsedJsonData["visible"]
    end
    if parsedJsonData["z_order"] ~= nil then
        print("UiItemBase:extractUiItemBaseReplicatorData => visible")
        self.properties.z_order.value = parsedJsonData["z_order"]
    end
    if parsedJsonData["width"] ~= nil then
        self.properties.width.value = parsedJsonData["width"]
    end
    if parsedJsonData["height"] ~= nil then
        self.properties.height.value = parsedJsonData["height"]
    end
    if parsedJsonData["horizontalCenterOffset"] ~= nil then
        self.properties.horizontalCenterOffset.value = parsedJsonData["horizontalCenterOffset"]
    end
    if parsedJsonData["verticalCenterOffset"] ~= nil then
        self.properties.verticalCenterOffset.value = parsedJsonData["verticalCenterOffset"]
    end
    if parsedJsonData["anchors"] ~= nil then
        --self.properties.z_order.value
        local anchors = parsedJsonData["anchors"]
    end
end

function UiItemBase:getUiItemBaseDataToReplicator()
    local propertiesData = {}
    local isPropsDirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    return propertiesData, isPropsDirty
end

function UiItemBase:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

function UiItemBase:setZOrder(z_order)
    if self.properties.z_order.value ~= z_order then
        self.properties.z_order.value = z_order
        self.properties.z_order.dirty = true
    end
end

function UiItemBase:setWidth(width)
    if self.properties.width.value ~= width then
        self.properties.width.value = width
        self.properties.width.dirty = true
    end
end

function UiItemBase:setHeight(height)
    if self.properties.height.value ~= height then
        self.properties.height.value = height
        self.properties.height.dirty = true
    end
end

function UiItemBase:setVerticalCenterOffset(verticalCenterOffset)
    if self.properties.verticalCenterOffset.value ~= verticalCenterOffset then
        self.properties.verticalCenterOffset.value = verticalCenterOffset
        self.properties.verticalCenterOffset.dirty = true
    end
end

function UiItemBase:setHorizontalCenterOffset(horizontalCenterOffset)
    if self.properties.horizontalCenterOffset.value ~= horizontalCenterOffset then
        self.properties.horizontalCenterOffset.value = horizontalCenterOffset
        self.properties.horizontalCenterOffset.dirty = true
    end
end

return UiItemBase
