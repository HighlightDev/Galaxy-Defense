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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiScrollList = UiItemBase:new()

function UiScrollList:new(host, name)
    assert(host ~= nil)

    local jsonParameters = nil
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_SCROLL_LIST,
                                                            jsonParameters)

    local scrollListProperties = {
        spacing = {value = 0, dirty = false},
        scroll_offset = {value = 0, dirty = false},
        scroll_speed = {value = 30, dirty = false}
    }

    local uiScrollListObj = UiScrollList.uiItemBaseClass.new(self)
    uiScrollListObj.typeName = "UiScrollList"
    uiScrollListObj.luaProxyId = luaProxyId
    uiScrollListObj.scrollListProperties = scrollListProperties
    uiScrollListObj.host = host

    return uiScrollListObj
end

function UiScrollList:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["spacing"] ~= nil then
                self.scrollListProperties.spacing.value = tonumber(parsedJson["spacing"])
            end
            if parsedJson["scroll_offset"] ~= nil then
                self.scrollListProperties.scroll_offset.value = tonumber(parsedJson["scroll_offset"])
            end
            if parsedJson["scroll_speed"] ~= nil then
                self.scrollListProperties.scroll_speed.value = tonumber(parsedJson["scroll_speed"])
            end
        end
    end
end

function UiScrollList:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.scrollListProperties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiScrollList:update(host, deltaTimeSec) end

function UiScrollList:setSpacing(spacing)
    assert(spacing ~= nil and type(spacing) == "number" and spacing >= 0)
    if self.scrollListProperties.spacing.value ~= spacing then
        self.scrollListProperties.spacing.value = spacing
        self.scrollListProperties.spacing.dirty = true
    end
end

function UiScrollList:setScrollOffset(offset)
    assert(offset ~= nil and type(offset) == "number")
    if self.scrollListProperties.scroll_offset.value ~= offset then
        self.scrollListProperties.scroll_offset.value = offset
        self.scrollListProperties.scroll_offset.dirty = true
    end
end

function UiScrollList:setScrollSpeed(speed)
    assert(speed ~= nil and type(speed) == "number" and speed > 0)
    if self.scrollListProperties.scroll_speed.value ~= speed then
        self.scrollListProperties.scroll_speed.value = speed
        self.scrollListProperties.scroll_speed.dirty = true
    end
end

function UiScrollList:getScrollOffset() return self.scrollListProperties.scroll_offset.value end

return UiScrollList
