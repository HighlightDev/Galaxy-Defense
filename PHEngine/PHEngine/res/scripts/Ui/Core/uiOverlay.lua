--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]]
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
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")
local ActionQueue = require("Ui/Core/actionQueue")

UiOverlay = {
}

function UiOverlay:createOverlay(host, overlayName, overlayCanvas)
    print("UiOverlay::createOverlay")
    assert(host ~= nil and overlayName ~= nil and overlayCanvas ~= nil)

    local uiOverlayJsonParameters = json.encode({ overlayName = overlayName, canvasLuaProxyId = overlayCanvas.luaProxyId })
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_OVERLAY,
        uiOverlayJsonParameters)

    local newObj = {
        luaProxyId = luaProxyId,
        overlayName = overlayName,
        overlayCanvas = overlayCanvas,
        isBackgroundOverlay = false,
        widgets = {},
        compoundWidgets = {},
        allWidgetLuaProxiesReady = false,
        allWidgetLuaProxiesReadyCallbacks = {},
        actionQueue = nil
    }

    newObj.actionQueue = ActionQueue:new()

    self.__index = self
    return setmetatable(newObj, self)
end

function UiOverlay:createBackgroundOverlay(host, overlayName, overlayCanvas)
    print("UiOverlay::createBackgroundOverlay")
    assert(host ~= nil and overlayName ~= nil and overlayCanvas ~= nil)

    local uiOverlayJsonParameters = json.encode({ overlayName = overlayName, canvasLuaProxyId = overlayCanvas.luaProxyId })
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
        CommonUiWidgetCreator.CommonUiWidgetType.UI_BACKGROUND_OVERLAY,
        uiOverlayJsonParameters)

    local newObj = {
        luaProxyId = luaProxyId,
        overlayName = overlayName,
        overlayCanvas = overlayCanvas,
        isBackgroundOverlay = true,
        widgets = {},
        compoundWidgets = {},
        allWidgetLuaProxiesReady = false,
        allWidgetLuaProxiesReadyCallbacks = {},
        actionQueue = nil
    }

    newObj.actionQueue = ActionQueue:new()

    self.__index = self
    return setmetatable(newObj, self)
end

function UiOverlay:addActionWithPredicate(action, predicate)
    assert(action ~= nil and type(action) == "function")
    assert(predicate ~= nil and type(predicate) == "function")
    self.actionQueue:addAction(action, predicate)
end

function UiOverlay:__gc(self)
end

function UiOverlay:getOverlayCanvas()
    return self.overlayCanvas
end

function UiOverlay:updateFromReplicatorData(host)
    self.overlayCanvas:updateFromReplicatorData(host)

    for _, value in pairs(self.widgets) do
        value:updateFromReplicatorData(host)
        if value.updateFromReplicatorMouseInputData ~= nil then
            value:updateFromReplicatorMouseInputData(host)
        end
    end
end

function UiOverlay:sendDataToReplicator(host)
    self.overlayCanvas:sendDataToReplicator(host)

    for _, value in pairs(self.widgets) do
        value:sendDataToReplicator(host)
    end
end

function UiOverlay:addWidget(widget)
    assert(widget ~= nil and type(widget) == "table" and widget.typeName ~= nil and type(widget.typeName) == "string")
    self.widgets[#self.widgets + 1] = widget
end

function UiOverlay:addCompoundWidget(compoundWidget)
    assert(compoundWidget ~= nil and type(compoundWidget) == "table" and compoundWidget.typeName == nil and
        compoundWidget.onCompoundWidgetInitialize ~= nil and
        type(compoundWidget.onCompoundWidgetInitialize) == "function")
    self.compoundWidgets[#self.compoundWidgets + 1] = compoundWidget
end

function UiOverlay:removeWidget(widget)
    assert(widget ~= nil and type(widget) == "table")
    if widget.typeName ~= nil and type(widget.typeName) == "string" then
        for index, iterate_widget in pairs(self.widgets) do
            if widget.luaProxyId == iterate_widget.luaProxyId then
                table.remove(self.widgets, index)
                break
            end
        end
    elseif widget.onCompoundWidgetInitialize ~= nil and
        type(widget.onCompoundWidgetInitialize) == "function" then
        assert(widget.backgroundTile.luaProxyReady)
        for index, iterate_widget in pairs(self.compoundWidgets) do
            if iterate_widget.backgroundTile ~= nil then
                assert(widget.backgroundTile ~= nil, "backgroundTile is null")
                if widget.backgroundTile.luaProxyId == iterate_widget.backgroundTile.luaProxyId then
                    table.remove(self.compoundWidgets, index)
                    break
                end
            end
        end
    end
end

function UiOverlay:subscribeOnAllWidgetLuaProxiesReady(callback)
    self.allWidgetLuaProxiesReadyCallbacks[#self.allWidgetLuaProxiesReadyCallbacks + 1] = callback
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
            for index, value in pairs(self.compoundWidgets) do
                value:onPreCompoundWidgetInitialize()
            end

            for _, callback in pairs(self.allWidgetLuaProxiesReadyCallbacks) do
                callback(host, self)
            end

            for _, value in pairs(self.compoundWidgets) do
                value:onCompoundWidgetInitialize()
            end
        end
    end

    if self.allWidgetLuaProxiesReady and self.actionQueue:hasPendingActions() then
        self.actionQueue:processActions() --process pending actions
    end

    for _, value in pairs(self.widgets) do
        value:update(host)
    end

    for _, value in pairs(self.compoundWidgets) do
        value:update(host)
    end
end

return UiOverlay
