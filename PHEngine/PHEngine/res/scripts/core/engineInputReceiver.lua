EngineInputReceiver = {
}

function EngineInputReceiver:new()
    print("EngineInputReceiver::new")
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
            self.onPressedKeyboardButtonCallback()
        end
        if self.hasReleasedKeyboardButtons and self.onReleasedKeyboardButtonCallback ~= nil then
            self.onReleasedKeyboardButtonCallback()
        end
    end
end

function EngineInputReceiver:__gc(self)
    print("EngineInputReceiver::dctor: ")
end

function EngineInputReceiver:subscribeOnPressedKeyboardButton(callback)
    self.onPressedKeyboardButtonCallback = callback
end

function EngineInputReceiver:subscribeOnReleasedKeyboardButton(callback)
    self.onReleasedKeyboardButtonCallback = callback
end

return EngineInputReceiver
