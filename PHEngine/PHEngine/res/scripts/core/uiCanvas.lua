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

local CommonUiWidgetCreator = require("commonUiWidgetCreator")
local json = require("json")

UiCanvas = {
}

function UiCanvas:new(host, originX, originY, width, height)
    print("UiCanvas::ctor")
    assert(host ~= nil and originX ~= nil and originY ~= nil and width ~= nil and height ~= nil)
    local canvasJsonParameters = json.encode({ originX = originX, originY = originY, width = width, height = height })
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_CANVAS,
        canvasJsonParameters)
    local newObj = {
        luaProxyId = luaProxyId,
        name = "",
        originX = originX,
        originY = originY,
        width = width,
        height = height,
        properties = {
            visible = {
                value = false,
                dirty = false
            }
        }
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function UiCanvas:__gc(self)
    print("UiCanvas::dctor => luaProxyId: " .. tostring(self.luaProxyId))
end

function UiCanvas:updateFromReplicatorData(host)
    local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
    if replicatorJsonData ~= "" then
        local parsedJson = json.decode(replicatorJsonData)
        if self.name == "" and parsedJson["name"] ~= nil then
            self.name = parsedJson["name"]
        end
        if parsedJson["visible"] ~= nil then
            self.visible = parsedJson["visible"]
        end
    end
end

function UiCanvas:sendDataToReplicator(host)
    local propertiesData = {}
    local propDataDirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            propDataDirty = true
            propertiesData[tostring(key)] = value.value
        end
    end

    if propDataDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiCanvas:update(host, deltaTime)
end

function UiCanvas:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

return UiCanvas
