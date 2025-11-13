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
        ,nightRight.jpg
		,nightLeft.jpg
		,nightTop.jpg
		,nightBottom.jpg
		,nightBack.jpg
		,nightFront.jpg
		]])

    _OpenAudioStreams(host, [[piano-loop2.ogg
        ]])

    _CreateFirstPersonCamera(host, "MainCamera", -- cameraName
    0, 0, _GetWindowWidth(host), _GetWindowHeight(host), -- viewPort
    Json.encode({
        projectionType = "Perspective",
        FoV = math.rad(60.0),
        AspectRatio = 16.0 / 9.0,
        NearPlane = 1.0,
        FarPlane = 500.0
    }), -- projectionInfo
    50.0, -- initPitchDeg
    20.0, -- initYawDeg
    0, 0, 0, -- init camera position
    1 -- is main camera on scene
    )

    local a_spaceSkyboxId = _CreateActor(host, "Actor", "SpaceSkyboxActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    local skyboxMatProxyId = _CreateMaterial(host, "SpaceSkyboxMaterial.m")
    _SetTextureToMaterial(host, skyboxMatProxyId,
        "nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg", "spaceTexture")

    _CreateAndAttachComponentToActor(host, a_spaceSkyboxId, "SkyboxComponent", Json.encode({
        gameObjectName = "SpaceSkyboxComponent",
        scale = {
            x = 250,
            y = 250,
            z = 250
        },
        materialProxyId = skyboxMatProxyId
    }))
end

function System_OnStart(host)
    CreateLevel(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
