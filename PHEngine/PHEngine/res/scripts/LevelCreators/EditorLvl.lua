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
    _LazyLoadResourcesAsync(host, [[arrow_right_1.png,
		Space_Station_COLOR.png,
		Space_Station_NORMAL.png,
		Space_Station_ROUGHNESS.jpg,
		Space_Station_METALLIC.jpg,
		space_station.obj,
		space_station_img.png,
        arrow_left.png,
        arrow_counter_clockwise.png,
        floppy-disk.png,
        route.png,
        wall.png,
        tower.png,
        perlin_noise_128x128.png,
        plane.obj,
        sphere.obj,
        ufo.obj,
        brick_mid.jpg,
	    brick_nm_mid.jpg,
        dummy_metallic_roughness.png
		]])

    _CreateActor(host, "Actor", "SceneCenterActorDummy", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    local a_lightId = _CreateActor(host, "Actor", "MainLightActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    _CreateAndAttachComponentToActor(host, a_lightId, "DirectionalLightComponent", Json.encode({
        gameObjectName = "MainLightComp",
        rotation = {x = 0, y = 0, z = 0},
        direction = {x = -0.2, y = -0.5, z = 0},
        ambient = {r = 0.2, g = 0.2, b = 0.2},
        diffuse = {r = 0.68, g = 0.5, b = 0.5},
        specular = {r = 0.4, g = 0.4, b = 0.4},
        is_enabled = true,
        is_visible = true
    }))

    -- local materialProxyId = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    -- _SetTextureToMaterial(host, materialProxyId, "brick_mid.jpg", "albedo")
    -- _SetTextureToMaterial(host, materialProxyId, "brick_nm_mid.jpg", "normalMap")
    -- _SetTextureToMaterial(host, materialProxyId, "dummy_metallic_roughness.png", "roughnessMap")
    -- _SetTextureToMaterial(host, materialProxyId, "dummy_metallic_roughness.png", "metallicMap")
    -- _SetFloatToMaterial(host, materialProxyId, 1.0, "uvScale")

    -- _CreateAndAttachComponentToActor(host, a_lightId, "StaticMeshComponent_Forward", Json.encode({
    --     gameObjectName = "LightVisual",
    --     meshName = "cylinder1.obj",
    --     translation = {
    --         x = 0,
    --         y = 0,
    --         z = 0
    --     },
    --     rotation = {
    --         x = 0,
    --         y = 0,
    --         z = 0
    --     },
    --     scale = {
    --         x = 1.0,
    --         y = 1.0,
    --         z = 1.0
    --     },
    --     materialProxyId = materialProxyId,
    --     luaScriptName = ""
    -- }))

    _CreateActor(host, "Actor", "SkyboxActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")
end

function System_OnStart(host) CreateLevel(host) end

function System_OnUpdate(host, deltaTimeSec) end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
