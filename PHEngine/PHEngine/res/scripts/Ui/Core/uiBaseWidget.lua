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
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")

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
        host = nil,
        typeName = "UiBaseWidget",
        parentClass = self,
        luaProxyReady = false,
        luaProxyReadyCallback = nil,
        luaProxyId = -1,
        widgetName = "",
        __gc = function(self)
            assert(self.host ~= nil and type(self.host) == "userdata")
            CommonUiWidgetCreator:destroyUiWidget(self.host, self.luaProxyId)
        end
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
