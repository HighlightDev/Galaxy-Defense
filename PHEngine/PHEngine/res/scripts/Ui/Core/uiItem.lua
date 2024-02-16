--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiItem = UiItemBase:new()

function UiItem:new(host, name)
    assert(host ~= nil)

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({ name = name })
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_ITEM,
        jsonParameters)

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
    local basePropertiesData, isPropsDirty = self:getUiItemBaseDataToReplicator()
    if isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(basePropertiesData))
    end
end

function UiItem:update(host)
end

return UiItem
