ActionQueue = {
    actions = {},
    predicates = {}
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

function ActionQueue:addAction(action, predicate)
    assert(action ~= nil and type(action) == "function")
    assert(predicate ~= nil and type(predicate) == "function")

    self.actions[#self.actions + 1] = action
    self.predicates[#self.predicates+1] = predicate
end

function ActionQueue:processActions()
    if self:hasPendingActions() then
        for index = 1, #self.actions, 1 do
            local action = self.actions[index]
            local predicate = self.predicates[index]
            if predicate() == true then
                action()
                table.remove(self.actions, index)
                table.remove(self.predicates, index)
            end
        end
    end
end

return ActionQueue
