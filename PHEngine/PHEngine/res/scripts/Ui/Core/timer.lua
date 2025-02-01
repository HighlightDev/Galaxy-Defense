TimerManager = {
    timers_id_counter = 0,
    timers = {},
    tickTimePassed = 0.0,
    lastTime = os.clock()
}

function TimerManager:new()
    local newObj = {
    }

    self.__index = self
    return setmetatable(newObj, self)
end

function TimerManager:createTimer(action, oneshot, intervalInSec)
    assert(action ~= nil and type(action) == "function")
    assert(oneshot ~= nil and type(oneshot) == "boolean")
    assert(intervalInSec ~= nil and type(intervalInSec) == "number")
    local timer = {
        id = TimerManager.timers_id_counter + 1,
        action = action,
        oneshot = oneshot,
        intervalInSec =
            intervalInSec,
        isRunning = false,
        timePassed = 0.0
    }
    TimerManager.timers[#TimerManager.timers + 1] = timer
    return timer.id
end

function TimerManager:startTimer(timer_id)
    for _, timer in pairs(TimerManager.timers) do
        if timer.id == timer_id then
            timer.isRunning = true
        end
    end
end

function TimerManager:stopTimer(timer_id)
    for _, timer in pairs(TimerManager.timers) do
        if timer.id == timer_id then
            timer.isRunning = false
        end
    end
end

function TimerManager:restartTimer(timer_id)
    for _, timer in pairs(TimerManager.timers) do
        if timer.id == timer_id then
            timer.timePassed = 0.0
            timer.isRunning = true
        end
    end
end

function TimerManager:updateTimers()
    self.tickTimePassed = os.clock() - self.lastTime
    for index, timer in pairs(self.timers) do
        if timer.isRunning then
            timer.timePassed = timer.timePassed + self.tickTimePassed
            if timer.timePassed >= timer.intervalInSec then
                timer.action()
                if timer.oneshot == true then
                    table.remove(TimerManager.timers, tonumber(index))
                else
                    timer.timePassed = 0
                end
            end
        end
    end
    self.lastTime = os.clock()
end

return TimerManager
