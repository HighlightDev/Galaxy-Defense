UiBaseWidget = {
}

function UiBaseWidget:new()
    local newObj = {
        parentClass = self,
        luaProxyReady = false,
        luaProxyReadyCallback = nil,
        luaProxyId = -1,
        uiWidgetName = ""
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function UiBaseWidget:subscribeOnLuaProxyReady(callback)
    self.luaProxyReadyCallback = callback
end

function UiBaseWidget:checkLuaProxyReady(host)
    if self.luaProxyReady ~= true then
        self.luaProxyReady = _IsLuaProxyReady(host, self.luaProxyId)
        if self.luaProxyReady and self.luaProxyReadyCallback ~= nil then
            self:luaProxyReadyCallback(host)
        end
    end
end

return UiBaseWidget