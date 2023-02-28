--[[ BEGIN *** this snippet have to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--[[ END   *** this snippet have to be inserted everywhere where your want to require custom modules  ***  END]]
--

local KeyboardKeys = require("keyboardKeys")
local json = require("json")

EngineInputReceiver = {
}

function EngineInputReceiver:new()
    print("EngineInputReceiver::ctor")
    local localObj = {
        subscribeToKeyboardEvents = true,
        subscribeToMouseEvents = true,
        canUpdate = true,
        hasPressedKeyboardButtons = false,
        hasReleasedKeyboardButtons = false,
        onPressedKeyboardButtonCallback = nil,
        onReleasedKeyboardButtonCallback = nil
    }
    self.__index = self
    return setmetatable(localObj, self)
end

function EngineInputReceiver:update(host)
    self.hasPressedKeyboardButtons = _HasPressedKeyboardButtons(host)
    self.hasReleasedKeyboardButtons = _HasReleasedKeyboardButtons(host)

    if self.subscribeToKeyboardEvents then
        if self.hasPressedKeyboardButtons and self.onPressedKeyboardButtonCallback ~= nil then
            local keyboardPressedKeyNames = {}
            if KeyboardKeys.ValuesToKeysPairs ~= nil then
                local jsonValuesToKeys = _GetKeyboardJsonData(host)
                local parsedJson = json.decode(jsonValuesToKeys)
                if parsedJson["pressed_keys"] ~= nil then
                    local keyboardValues = parsedJson["pressed_keys"]

                    for _, value in pairs(keyboardValues) do
                        table.insert(keyboardPressedKeyNames, KeyboardKeys.ValuesToKeysPairs[value])
                    end
                end
            end

            self.onPressedKeyboardButtonCallback(host, keyboardPressedKeyNames)
        end
        if self.hasReleasedKeyboardButtons and self.onReleasedKeyboardButtonCallback ~= nil then
            self.onReleasedKeyboardButtonCallback()
        end
    end
end

function EngineInputReceiver:__gc(self)
    print("EngineInputReceiver::dctor")
end

function EngineInputReceiver:subscribeOnPressedKeyboardButton(callback)
    self.onPressedKeyboardButtonCallback = callback
end

function EngineInputReceiver:subscribeOnReleasedKeyboardButton(callback)
    self.onReleasedKeyboardButtonCallback = callback
end

return EngineInputReceiver
