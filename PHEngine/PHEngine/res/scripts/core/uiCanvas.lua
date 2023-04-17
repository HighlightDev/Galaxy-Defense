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
local CommonUiWidgetCreator = require("commonUiWidgetCreator")
local json = require("3rdparty/json")

UiCanvas = UiBaseWidget:new()

function UiCanvas:new(host, originX, originY, width, height)
    print("UiCanvas::ctor")
    assert(host ~= nil and originX ~= nil and originY ~= nil and width ~= nil and height ~= nil)
    local canvasJsonParameters = json.encode({ originX = originX, originY = originY, width = width, height = height })
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_CANVAS,
        canvasJsonParameters)

    local canvasProperties = {
        visible = {
            value = false,
            dirty = false
        }
    }

    local canvasObj = UiCanvas.parentClass.new(self)
    canvasObj.luaProxyId = luaProxyId
    canvasObj.originX = originX
    canvasObj.originY = originY
    canvasObj.width = width
    canvasObj.height = height
    canvasObj.properties = canvasProperties
    return canvasObj
end

function UiCanvas:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
    if replicatorJsonData ~= "" then
        local parsedJson = json.decode(replicatorJsonData)
        if parsedJson["visible"] ~= nil then
            self.properties.visible.value = parsedJson["visible"]
            print("UiCanvas:updateFromReplicatorData => visible: " .. tostring(self.properties.visible.value))
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
            value.dirty = false
        end
    end

    if propDataDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiCanvas:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

function UiCanvas:update(host, deltaTime)
end

return UiCanvas
