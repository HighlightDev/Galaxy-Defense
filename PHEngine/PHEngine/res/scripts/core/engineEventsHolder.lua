EngineEventsHolder = {
    enqueueJobPolicy = {
        IF_DUPLICATE_NO_PUSH = 0,
        IF_DUPLICATE_REPLACE = 1,
        PUSH_ANYWAY = 2
    }
}

function EngineEventsHolder:sendPauseGameThreadEvent(host, enqueueJobPolicy, isPause)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(isPause ~= nil)
    _SendPauseGameThreadEvent(host, enqueueJobPolicy, isPause)
end

function EngineEventsHolder:sendExitGameThreadEvent(host, enqueueJobPolicy)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    _SendExitGameThreadEvent(host, enqueueJobPolicy)
end

function EngineEventsHolder:sendLoadLevelGameThreadEvent(host, enqueueJobPolicy, levelName)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(levelName ~= nil and type(levelName) == "string")
    _SendLoadLevelGameThreadEvent(host, enqueueJobPolicy, levelName)
end

return EngineEventsHolder
