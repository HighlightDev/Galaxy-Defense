UiBaseWidget = {
    EnginePropertyType = {
        Undefined = 0,
        Float = 1,
        Vec3 = 2,
        Boolean = 3,
		Integer = 4
    },
    AnimationInterpolationFunctionType = {
        LINEAR = 0
    }
}

function UiBaseWidget:new()
    local newObj = {
        typeName = "UiBaseWidget",
        parentClass = self,
        luaProxyReady = false,
        luaProxyReadyCallback = nil,
        luaProxyId = -1,
        widgetName = ""
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
        self.widgetName = _GetUiWidgetName(host, self.luaProxyId)
        if self.luaProxyReady == true then
            print("UiBaseWidget:checkLuaProxyReady => " .. tostring(self.widgetName) .. " is now ready.")
            if self.luaProxyReadyCallback ~= nil then
                self.luaProxyReadyCallback(host)
            end
        end
    end
end

return UiBaseWidget
