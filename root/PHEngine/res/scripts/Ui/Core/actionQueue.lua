ActionQueue = {}

function ActionQueue:new()
    local newObj = {actions = {}, predicates = {}}

    self.__index = self
    return setmetatable(newObj, self)
end

function ActionQueue:hasPendingActions() return #self.actions > 0 end

function ActionQueue:addAction(action, predicate)
    assert(action ~= nil and type(action) == "function", debug.traceback())
    assert(predicate ~= nil and type(predicate) == "function", debug.traceback())

    self.actions[#self.actions + 1] = action
    self.predicates[#self.predicates + 1] = predicate
end

function ActionQueue:processActions()
    local index = 1
    while index <= #self.actions do
        if self.predicates[index]() == true then
            -- remove before the invocation, because the action can add new actions to the queue
            local action = table.remove(self.actions, index)
            table.remove(self.predicates, index)
            action()
        else
            index = index + 1
        end
    end
end

return ActionQueue
