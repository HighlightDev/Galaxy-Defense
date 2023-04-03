--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]]
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
local json = require("3rdparty/json")

UiOverlay = {
}

function UiOverlay:new(host, overlayName, overlayCanvas)
    print("UiOverlay::ctor")
    assert(host ~= nil and overlayName ~= nil and overlayCanvas ~= nil)

    local uiOverlayJsonParameters = json.encode({ overlayName = overlayName, canvasLuaProxyId = overlayCanvas.luaProxyId })
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_OVERLAY,
        uiOverlayJsonParameters)

    local newObj = {
        luaProxyId = luaProxyId,
        overlayName = overlayName,
        overlayCanvas = overlayCanvas,
        widgets = {}
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function UiOverlay:__gc(self)
    print("UiOverlay::dctor => luaProxyId: " .. tostring(self.luaProxyId))
end

function UiOverlay:updateFromReplicatorData(host)
    self.overlayCanvas:updateFromReplicatorData(host)

    for key, value in pairs(self.widgets) do
        value:updateFromReplicatorData(host)
    end
end

function UiOverlay:sendDataToReplicator(host)
    self.overlayCanvas:sendDataToReplicator(host)

    for key, value in pairs(self.widgets) do
        value:sendDataToReplicator(host)
    end
end

function UiOverlay:update(host, deltaTime)
    self.overlayCanvas:update(host, deltaTime)
end

function UiOverlay:addWidget(widget)
    assert(widget ~= nil)
    table.insert(self.widgets, widget)
end

return UiOverlay
