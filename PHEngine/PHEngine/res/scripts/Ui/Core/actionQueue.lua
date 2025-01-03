ActionQueue = {
    actions = {}
}

function ActionQueue:new()
    local newObj = {
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function ActionQueue:hasPendingActions()
    return #self.actions > 0
end

function ActionQueue:addAction(action)
    assert(action ~= nil and type(action) == "function")
    self.actions[#self.actions + 1] = action
end

function ActionQueue:processActions()
    if self:hasPendingActions() then
        for _, action in ipairs(self.actions) do
            action()
        end
        self.actions = {}
    end
end

return ActionQueue
