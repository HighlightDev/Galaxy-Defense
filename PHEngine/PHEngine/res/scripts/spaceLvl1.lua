--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
	local slash = package.config:sub(1,1)
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

local Vec3 = require("core/vec3")

function CreateTestLevel(host)

	_LazyLoadResourcesAsync(host,
		[[arrow_right_1.png
		,nimbus_mono.png
		,scaled_down_heart.png
		,background_shield.png
		,weapon_missile.png
		,weapon_missile_2.png
		,weapon_missile_3.png
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
		,piano-loop2.wav
		,planet_1.png
		,planet_2.png
		,planet_3.png
		,planet_4.png
		,planet_5.png
		,planet_6.png
		]])

	local translation = Vec3:new(0, 0, 0)
	local rotation = Vec3:new(0, 0, 0)
	local scale = Vec3:new(1, 1, 1)

	local a_light = _CreateActor(host, "MainLightActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1)

	if a_light ~= nil then
		local rotation = { x = 0, y = 0, z = 0 }
		local direction = { x = -0.2, y = -0.5, z = 0 }
		local ambient = { x = 0.2, y = 0.2, z = 0.2 }
		local diffuse = { x = 0.68, y = 0.5, z = 0.5 }
		local specular = { x = 0.4, y = 0.4, z = 0.4 }

		--local dirShadowInfo = _CreateLightProjectionShadowInfo(host, 512, "direct_light")

		local d_dirLight = _CreateDirLightComponentData(host, "MainLightComp",
			rotation.x, rotation.y, rotation.z,
			direction.x, direction.y, direction.z,
			ambient.x, ambient.y, ambient.z,
			diffuse.x, diffuse.y, diffuse.z,
			specular.x, specular.y, specular.z,
			nil
		)

		local c_dirLight = _CreateComponent(host, "DirectionalLightComponent", d_dirLight)
		_AttachComponentToActor(host, "MainLightActor", c_dirLight)
	end
	
	local a_skybox = _CreateActor(host, "SkyboxActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1)

	local a_spaceship = _CreateActor(host, "SpaceshipActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1)

	if a_spaceship ~= nil then
		local mat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, mat, "spaceship_albedo.jpg", "albedo")
		_SetTextureToMaterial(host, mat, "spaceship_normal.jpg", "normalMap")
		_SetTextureToMaterial(host, mat, "spaceship_roughness.jpg", "roughnessMap")
		_SetTextureToMaterial(host, mat, "spaceship_metallic.jpg", "metallicMap")
		_SetFloatToMaterial(host, mat, 1.0, "uvScale")

		local d_spaceship = _CreateMeshComponentData(host, "spaceshipMeshComponent", "spaceship.obj",
			0, 0, 0,
			0, 180, 0,
			5, 5, 5,
			"",
			mat)

		local c_spaceship = _CreateComponent(host, "StaticMeshComponent", d_spaceship)
		_AttachComponentToActor(host, "SpaceshipActor", c_spaceship)
	end

end

function System_OnStart(host)
	CreateTestLevel(host)
end

function System_OnUpdate(host, deltaTime)
	print("LUA => deltaTime:", deltaTime)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
