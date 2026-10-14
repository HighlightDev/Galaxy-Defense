--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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
        local _, endindex = string.find(unixLikePath, "scripts/")
        unixLikePath = string.sub(unixLikePath, 1, endindex)
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]

local Json = require("Ui/Core/3rdparty/json")

local function CreateLevel(host)
    _LazyLoadResourcesAsync(host, [[arrow_right_1.png
        ]])

    _OpenAudioStreams(host, [[piano-loop2.ogg
        ]])

    -- A forward-facing camera is all we need; the procedural space background
    -- is drawn by MainMenuLevel::CreateBackgroundBillboard on a billboard whose
    -- view/projection matrices are forced to identity in C++, so the quad
    -- always covers the full viewport regardless of camera pose. The
    -- Lua-facing BillboardComponent factory hard-codes those matrices to
    -- passthrough, which is why the background can't be set up here.
    _CreateFirstPersonCamera(host, "MainCamera", -- cameraName
    0, 0, _GetWindowWidth(host), _GetWindowHeight(host), -- viewPort
    Json.encode({
        projectionType = "Perspective",
        FoV = math.rad(60.0),
        AspectRatio = 16.0 / 9.0,
        NearPlane = 1.0,
        FarPlane = 500.0
    }), -- projectionInfo
    0.0, -- initPitchDeg
    0.0, -- initYawDeg
    0, 0, 0, -- init camera position
    1 -- is main camera on scene
    )

    -- Empty actor that the C++ side (MainMenuLevel::CreateBackgroundBillboard)
    -- looks up by name and attaches the procedural background billboard to.
    _CreateActor(host, "Actor", "MainMenuBackgroundActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")
end

function System_OnStart(host) CreateLevel(host) end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
