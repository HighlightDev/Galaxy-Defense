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
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]

local function initialize(host)
    _LazyLoadResourcesAsync(host,
        [[arrow_right_1.png
		,nimbus_mono.png
		]])

    _CreateFirstPersonCamera(host,
        "MainCamera",                                        --cameraName
        0, 0, _GetWindowWidth(host), _GetWindowHeight(host), --viewPort
        50.0,                                                --initPitchDeg
        20.0,                                                --initYawDeg
        0, 0, 0,                                             --init camera position
        1                                                    -- is main camera on scene
    )
end

function System_OnStart(host)
    initialize(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
