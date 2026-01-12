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

local Vec3 = require("Ui/Core/vec3")
local Json = require("Ui/Core/3rdparty/json")

function CreateLevel(host)
    _LazyLoadResourcesAsync(host, [[arrow_right_1.png
		,scaled_down_heart.png
		,background_shield.png
		,weapon_missile.png
		,perlin_noise_128x128.png
		,nightLeft.jpg
		,spaceship_albedo.jpg
		,spaceship_normal.jpg
		,spaceship_roughness.jpg
		,spaceship_metallic.jpg
		,solar_cells_normal_512.jpg
		,solar_cells_roughness_512.jpg
		,solar_cells_metallic_512.jpg
		,missile1_albedo.png
		,water_dudv.jpg
		,circle_mask1.png
		,Ice_Cracked_albedo.jpg
		,Ice_Cracked_normal.jpg
		,Ice_Cracked_metallic.jpg
		,Ice_Cracked_roughness.jpg
		,Asteroid_albedo.jpg
		,Asteroid_normal.jpg
		,Asteroid_roughness.jpg
		,Asteroid_metallic.jpg
		,sphere.obj
		,plane.obj
		,spaceship.obj
		,missile1_model.fbx
		,asteroid.fbx
		,explosion1.ogg
		,Space_Station_COLOR.png
		,Space_Station_NORMAL.png
		,Space_Station_ROUGHNESS.jpg
		,Space_Station_METALLIC.jpg
		,space_station.obj
		,space_station_img.png
		,ufo.obj
		,check.png
		,cancel.png
		,plus.png
		,minus.png
		,flag-banner-fold.png
		,default_circle_mask.png
		,skull.png
		,warning.png
        ,hammer.png
        ,trash.png
		]])

    _CreateActor(host, "Actor", "SceneCenterActorDummy", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    local a_lightId = _CreateActor(host, "Actor", "MainLightActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    _CreateAndAttachComponentToActor(host, a_lightId, "DirectionalLightComponent", Json.encode({
        gameObjectName = "MainLightComp",
        rotation = {x = 0, y = 0, z = 0},
        direction = {x = -0.2, y = -0.5, z = 0},
        ambient = {r = 0.8, g = 0.2, b = 1.0},
        diffuse = {r = 0.68, g = 0.5, b = 0.5},
        specular = {r = 0.4, g = 0.4, b = 0.4},
        is_enabled = true,
        is_visible = true
    }))

    _CreateActor(host, "Actor", "SkyboxActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")
end

function CreateLevelProgressStages(host)
    local stagesQueue = {};
    local trackers = {}
    trackers[1] = {
        type = "MissedSpaceshipsTracker",
        spaceships_count = 20,
        hint = string.format("Не пропустите более %d космических кораблей", 20)
    }
    trackers[2] = {
        type = "DestroySpaceshipsTracker",
        spaceships_count = 10,
        hint = string.format("Уничтожьте %d космических кораблей", 10)
    }
    local stage = {name = "weak_spaceships_attack", trackers = trackers}
    stagesQueue[1] = stage

    _SetLevelProgressStagesQueue(host, Json.encode(stagesQueue))
end

function System_OnStart(host)
    CreateLevel(host)
    CreateLevelProgressStages(host)
end

function System_OnUpdate(host, deltaTimeSec) end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
