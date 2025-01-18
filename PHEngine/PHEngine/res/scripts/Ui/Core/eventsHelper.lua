EventsHelper = {
    enqueueJobPolicy = {
        IF_DUPLICATE_NO_PUSH = 0,
        IF_DUPLICATE_REPLACE = 1,
        PUSH_ANYWAY = 2
    }
}

function EventsHelper:sendPauseGameThreadEvent(host, enqueueJobPolicy, isPause)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(isPause ~= nil)
    _SendPauseGameThreadEvent(host, enqueueJobPolicy, isPause)
end

function EventsHelper:sendExitGameThreadEvent(host, enqueueJobPolicy)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    _SendExitGameThreadEvent(host, enqueueJobPolicy)
end

function EventsHelper:sendLoadLevelGameThreadEvent(host, enqueueJobPolicy, levelName)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(levelName ~= nil and type(levelName) == "string")
    _SendLoadLevelGameThreadEvent(host, enqueueJobPolicy, levelName)
end

function EventsHelper:sendChangeGameModeGameThreadEvent(host, enqueueJobPolicy, gameModeType)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(gameModeType ~= nil and type(gameModeType) == "number")
    _SendChangeGameModeGameThreadEvent(host, enqueueJobPolicy, gameModeType)
end

function EventsHelper:sendChangeEditModeGameThreadEvent(host, enqueueJobPolicy, editModeType)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(editModeType ~= nil and type(editModeType) == "number")
    _SendChangeEditModeGameThreadEvent(host, enqueueJobPolicy, editModeType)
end

function EventsHelper:sendBroadcastGameThreadEvent(host, enqueueJobPolicy, eventHeader, jsonParameters)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    assert(eventHeader ~= nil and type(eventHeader) == "string")
    assert(jsonParameters ~= nil and type(jsonParameters) == "string")
    _SendBroadcastGameThreadEvent(host, enqueueJobPolicy, eventHeader, jsonParameters)
end

function EventsHelper:sendRestartLevelGameThreadEvent(host, enqueueJobPolicy)
    assert(enqueueJobPolicy >= 0 or enqueueJobPolicy <= 2, "enqueueJobPolicy value must be [0; 2]")
    _SendRestartLevelGameThreadEvent(host, enqueueJobPolicy)
end

return EventsHelper
