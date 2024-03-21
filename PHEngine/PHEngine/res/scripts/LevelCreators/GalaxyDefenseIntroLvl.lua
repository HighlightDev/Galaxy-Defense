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

function CreateTestLevel(host)
	_LazyLoadResourcesAsync(host,
		[[arrow_right_1.png
		,nimbus_mono.png
		,scaled_down_heart.png
		,background_shield.png
		,weapon_missile.png
		,perlin_noise.png
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
		]])

	_CreateActor(host, "Actor",
		"SceneCenterActorDummy",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local a_lightId = _CreateActor(host, "Actor",
		"MainLightActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	_CreateAndAttachComponentToActor(host, a_lightId, "DirectionalLightComponent",
		Json.encode(
			{
				gameObjectName = "MainLightComp",
				rotation = { x = 0, y = 0, z = 0 },
				direction = { x = -0.2, y = -0.5, z = 0 },
				ambient = { r = 0.2, g = 0.2, b = 0.2 },
				diffuse = { r = 0.68, g = 0.5, b = 0.5 },
				specular = { r = 0.4, g = 0.4, b = 0.4 }
			}
		))

	_CreateActor(host, "Actor",
		"SkyboxActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local spaceship_a = _CreateActor(host, "Actor",
		"SpaceshipActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local spaceshipMatProxyId = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, spaceshipMatProxyId, "Space_Station_COLOR.png", "albedo")
	_SetTextureToMaterial(host, spaceshipMatProxyId, "Space_Station_NORMAL.png", "normalMap")
	_SetTextureToMaterial(host, spaceshipMatProxyId, "Space_Station_METALLIC.jpg", "roughnessMap")
	_SetTextureToMaterial(host, spaceshipMatProxyId, "Space_Station_ROUGHNESS.jpg", "metallicMap")
	_SetFloatToMaterial(host, spaceshipMatProxyId, 1.0, "uvScale")

	_CreateAndAttachComponentToActor(host, spaceship_a, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "SpaceshipMeshComponent",
				meshName = "space_station.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 7, y = 7, z = 7 },
				luaScriptName = "",
				materialProxyId = spaceshipMatProxyId
			}
		))
end

function System_OnStart(host)
	CreateTestLevel(host)
end

function System_OnUpdate(host, deltaTime)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
