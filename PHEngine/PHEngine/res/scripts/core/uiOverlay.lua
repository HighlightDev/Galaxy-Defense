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
        widgets = {},
        allWidgetLuaProxiesReady = false,
        allWidgetLuaProxiesReadyCallback = nil
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function UiOverlay:__gc(self)
    print("UiOverlay::dctor => luaProxyId: " .. tostring(self.luaProxyId))
end

function UiOverlay:updateFromReplicatorData(host)
    self.overlayCanvas:updateFromReplicatorData(host)

    for _, value in pairs(self.widgets) do
        value:updateFromReplicatorData(host)
    end
end

function UiOverlay:sendDataToReplicator(host)
    self.overlayCanvas:sendDataToReplicator(host)

    for _, value in pairs(self.widgets) do
        value:sendDataToReplicator(host)
    end
end

function UiOverlay:addWidget(widget)
    assert(widget ~= nil)
    table.insert(self.widgets, widget)
end

function UiOverlay:subscribeOnAllWidgetLuaProxiesReady(callback)
    self.allWidgetLuaProxiesReadyCallback = callback
end

function UiOverlay:update(host, deltaTime)
    self.overlayCanvas:update(host, deltaTime)

    if self.allWidgetLuaProxiesReady ~= true then
        local allProxiesReady = true
        for _, value in pairs(self.widgets) do
            if value.luaProxyReady ~= true then
                allProxiesReady = false
            end
        end
        if allProxiesReady and self.overlayCanvas.luaProxyReady then
            self.allWidgetLuaProxiesReady = true
            if self.allWidgetLuaProxiesReadyCallback ~= nil then
                self.allWidgetLuaProxiesReadyCallback(host, self)
            end
        end
    end

    for _, value in pairs(self.widgets) do
        value:update(host)
    end
end

return UiOverlay
