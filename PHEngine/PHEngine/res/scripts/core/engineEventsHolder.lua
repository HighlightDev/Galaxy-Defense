EngineEventsHolder = {
    enqueueJobPolicy = {
        IF_DUPLICATE_NO_PUSH = 0,
        IF_DUPLICATE_REPLACE = 1,
        PUSH_ANYWAY = 2
    }
}

function EngineEventsHolder:sendPauseGameThreadEvent(host,  enqueueJobPolicy, isPause)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(isPause ~= nil)
    return _SendPauseGameThreadEvent(host, enqueueJobPolicy, isPause)
end

return EngineEventsHolder
