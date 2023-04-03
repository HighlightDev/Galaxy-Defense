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
local UiItemBase = require("uiItemBase")
local CommonUiWidgetCreator = require("commonUiWidgetCreator")
local json = require("3rdparty/json")

UiItem = UiItemBase:new()

function UiItem:new(host)
    assert(host ~= nil)
    print("UiItem::ctor")

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_ITEM)

    local uiItemObj = UiItem.uiItemBaseClass.new(self)
    uiItemObj.luaProxyId = luaProxyId

    return uiItemObj
end

function UiItem:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)
        end
    end
end

function UiItem:sendDataToReplicator(host)
    local baseData = UiItem:getUiItemBaseDataToReplicator()
    local propertiesData = {}
    local propDataDirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            propDataDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    if propDataDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

return UiItem
