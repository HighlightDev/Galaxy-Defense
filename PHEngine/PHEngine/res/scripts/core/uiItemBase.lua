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
local json = require("json")

UiItemBase = UiBaseWidget:new()

function UiItemBase:new()
    print("UiItemBase::ctor")

    local uiItemBaseObj = UiItemBase.parentClass.new(self)
    uiItemBaseObj.uiItemBaseClass = self
    uiItemBaseObj.uiWidgetName = ""
    uiItemBaseObj.properties = {
        visible = {
            value = false,
            dirty = false
        }
    }

    return uiItemBaseObj
end

function UiItemBase:extractUiItemBaseReplicatorData(parsedJsonData)
    if self.uiWidgetName == "" and parsedJsonData["uiWidgetName"] ~= nil then
        self.uiWidgetName = parsedJsonData["uiWidgetName"]
    end
    if parsedJsonData["visible"] ~= nil then
        self.properties.visible.value = parsedJsonData["visible"]
    end
end

function UiItemBase:getUiItemBaseDataToReplicator()
    local propertiesData = {}
    propertiesData.dirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            propertiesData.dirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    return propertiesData
end

function UiItemBase:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

return UiItemBase
