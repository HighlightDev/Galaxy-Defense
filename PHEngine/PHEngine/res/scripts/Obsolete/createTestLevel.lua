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

local Json = require("Ui/Core/3rdparty/json")

local PhysicsBodyType = {
	STATIC = 0,
	DYNAMIC = 1,
	KINEMATIC = 2,
	GHOST = 3
}

function CreateTestLevel(host)
	_LazyLoadResourcesAsync(host,
		[[brick_mid.jpg
	,brick_nm_mid.jpg
	,city_house_2_Col.jpg
	,city_house_2_Nor.jpg
	,city_house_2_Spec.png
	,diffuse.png
	,dummy_nm.png
	,dayRight.jpg
	,dayLeft.jpg
	,dayTop.jpg
	,dayBottom.jpg
	,dayBack.jpg
	,dayFront.jpg
	,nightRight.jpg
	,nightLeft.jpg
	,nightTop.jpg
	,nightBottom.jpg
	,nightBack.jpg
	,nightFront.jpg
	,water_dudv.jpg
	,Brick_Medieval_albedo.jpg
	,Brick_Medieval_normal.jpg
	,Brick_Medieval_roughness.jpg
	,Brick_Medieval_metallic.jpg
	,dummy_metallic_roughness.png
	,witcher.obj
	,italian_house_1.obj
	,spaceship.obj
	,spaceship_albedo.jpg
	,spaceship_normal.jpg
	,spaceship_roughness.jpg
	,spaceship_metallic.jpg
	,solar_cells_albedo_512.jpg
	,solar_cells_normal_512.jpg
	,solar_cells_roughness_512.jpg
	,solar_cells_metallic_512.jpg
	,cube.obj
	,arrow_right_1.png
	,tina.fbx
	]])


	_OpenAudioStreams(host,
		[[piano-loop2.ogg
        ]])

	-- *********************************CREATE MAIN CAMERA******************************** --
	_CreateThirdPersonCamera(host, "MainCamera",
		0, 0, _GetWindowWidth(host), _GetWindowHeight(host),
		Json.encode({
			projectionType = "Perspective",
			FoV = math.rad(60.0),
			AspectRatio = 16.0 / 9.0,
			NearPlane = 1.0,
			FarPlane = 500.0
		}), --projectionInfo
		50.0,
		20.0,
		220.0,
		0, 0, 0,
		1) -- is main camera on scene

	_SetCameraThirdPersonTarget(host, "MainCamera", "SkeletActor")

	-- ****************************SKYBOX***************************** --
	local a_spaceSkyboxId = _CreateActor(host, "Actor",
		"SkyboxActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local skyboxMat = _CreateMaterial(host, "SkyboxMaterial.m")
	_SetTextureToMaterial(host, skyboxMat, "dayRight.jpg,dayLeft.jpg,dayTop.jpg,dayBottom.jpg,dayBack.jpg,dayFront.jpg",
		"dayTexture")
	_SetTextureToMaterial(host, skyboxMat,
		"nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg", "nightTexture")
	_SetBindingToMaterial(host, skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTime")
	_SetFloatToMaterial(host, skyboxMat, 0.01, "mul_coef")

	_CreateAndAttachComponentToActor(host, a_spaceSkyboxId, "SkyboxComponent",
		Json.encode(
			{
				gameObjectName = "SpaceSkyboxComponent",
				scale = { x = 250, y = 250, z = 250 },
				materialProxyId = skyboxMat
			}
		))

	-- ****************************PLANAR REFLECTION***************************** --

	_CreatePlanarReflectionComponent(host,
		Json.encode(
			{
				gameObjectName = "PlanarReflectionComponent",
				cameraName = "MainCamera",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 1, y = 1, z = 1 },
				viewPortX = 0,
				viewPortY = 0,
				viewPortWidth = _GetWindowWidth(host),
				viewPortHeight = _GetWindowHeight(host),
			}
		))

	-- -- ****************************LIGHT***************************** --

	local a_dirLightId = _CreateActor(host, "Actor",
		"DirectionalLightActor",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	_CreateAndAttachComponentToActor(host, a_dirLightId, "DirectionalLightComponent",
		Json.encode(
			{
				gameObjectName = "MainLightComp",
				rotation = { x = 0, y = 0, z = 0 },
				direction = { x = -0.5, y = -0.5, z = 0 },
				ambient = { r = 0.2, g = 0.2, b = 0.2 },
				diffuse = { r = 1.68, g = 1.5, b = 1.5 },
				specular = { r = 0.4, g = 0.4, b = 0.4 },
				is_enabled = 0,
				is_visible = 0,
				shadowAtlasSize = 256
			}
		))
	
	local a_pointLightId = _CreateActor(host, "Actor",
		"PointlLightActor",
		10, 10, 10,
		0, 0, 0,
		1, 1, 1,
		"")
	
		_CreateAndAttachComponentToActor(host, a_pointLightId, "PointLightComponent",
		Json.encode(
			{
				gameObjectName = "PointLightComp",
				translation = {x = 0, y = 0, z = 0},
				ambient = { r = 0.2, g = 0.2, b = 0.2 },
				diffuse = { r = 0.68, g = 0.5, b = 0.2 },
				specular = { r = 0.4, g = 0.4, b = 0.4 },
				attenuation = {x = 1, y = 1, z = 1},
				radianceRadius = 100.0,
				is_enabled = 1,
				is_visible = 1,
				shadowAtlasSize = 256
			}
		))

	-- if lightActor ~= nil then
	--local pointShadowInfo = _CreateLightProjectionShadowInfo(host, 256, "point_light")
	--local pointLightComponentData = _CreatePointLightComponentData(host, "SecondaryLightComp",
	--	pointLTranslation.x, pointLTranslation.y, pointLTranslation.z,
	--	ambient.x, ambient.y, ambient.z,
	--	diffuse.x, diffuse.y, diffuse.z,
	--	specular.x, specular.y, specular.z,
	--	attenuation.x, attenuation.y, attenuation.z,
	--	100.0,
	--	pointShadowInfo
	--	)
	--local pointLightComponent = _CreateComponent(host, "PointLightComponent", pointLightComponentData)
	--_AttachComponentToActor(host, "MainLightActor", pointLightComponent)

	--local spotlightShadowInfo = _CreateLightProjectionShadowInfo(host, 256, "spotlight")
	--local spotlightCD = _CreateSpotlightComponentData(host,
	--	"spotlightComp",
	--	-15, 5, 0,
	--	rotation.x, 0, rotation.z,
	--	ambient.x, ambient.y, ambient.z,
	--	diffuse.x, diffuse.y, diffuse.z,
	--	specular.x, specular.y, specular.z,
	--	attenuation.x, attenuation.y, attenuation.z,
	--	100.0,
	--	0.75,
	--	spotlightShadowInfo)
	--local spotlightComponent = _CreateComponent(host, "SpotlightComponent", spotlightCD)
	--_AttachComponentToActor(host, "MainLightActor", spotlightComponent)
	--end

	-- **************************** GROUND ***************************** --
	local a_ground = _CreateActor(host, "Actor",
		"Ground",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local groundMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, groundMat, "brick_mid.jpg", "albedo")
	_SetTextureToMaterial(host, groundMat, "brick_nm_mid.jpg", "normalMap")
	_SetTextureToMaterial(host, groundMat, "dummy_metallic_roughness.png", "roughnessMap")
	_SetTextureToMaterial(host, groundMat, "dummy_metallic_roughness.png", "metallicMap")
	_SetFloatToMaterial(host, groundMat, 5.0, "uvScale")

	_CreateAndAttachComponentToActor(host, a_ground, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "GroundMeshComponent",
				meshName = "cube.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 50, y = 1, z = 50 },
				luaScriptName = "",
				materialProxyId = groundMat
			}
		))

	_CreateAndAttachComponentToActor(host, a_ground, "RigidBodyPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "GroundPhysicsComponent",
				collisionShape = "box",
				halfExtent = { x = 50, y = 1, z = 50 },
				physicsBodyType = PhysicsBodyType.STATIC,
				mass = 0.0
			}
		))

	-- **************************** Platform ***************************** --
	local a_platform = _CreateActor(host, "Actor",
		"Platform",
		0, 10, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local platformMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, platformMat, "brick_mid.jpg", "albedo")
	_SetTextureToMaterial(host, platformMat, "brick_nm_mid.jpg", "normalMap")
	_SetTextureToMaterial(host, platformMat, "dummy_metallic_roughness.png", "roughnessMap")
	_SetTextureToMaterial(host, platformMat, "dummy_metallic_roughness.png", "metallicMap")
	_SetFloatToMaterial(host, platformMat, 1.0, "uvScale")

	_CreateAndAttachComponentToActor(host, a_platform, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "PlatformdMeshComponent",
				meshName = "cube.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 8, y = 1, z = 8 },
				luaScriptName = "",
				materialProxyId = platformMat
			}
		))

	_CreateAndAttachComponentToActor(host, a_platform, "PlatformTraverseComponent",
		Json.encode(
			{
				gameObjectName = "PlatformTraverseComponent",
				scriptName = "platformMovementComponentAction.lua",
				routePoints = {
					a1 = {
						translation = { x = 0, y = 0, z = 0 },
						rotation = { x = 0, y = 90, z = 0 },
						scale = { x = 1, y = 1, z = 1 },
						transitionTime = 1.8
					},
					a2 = {
						translation = { x = 10, y = 0, z = 0 },
						rotation = { x = 0, y = 90, z = 0 },
						scale = { x = 1, y = 1, z = 1 },
						transitionTime = 1.8
					},
					a3 = {
						translation = { x = 0, y = 0, z = 10 },
						rotation = { x = 0, y = 90, z = 0 },
						scale = { x = 1, y = 1, z = 1 },
						transitionTime = 1.8
					},
					a4 = {
						translation = { x = -10, y = 0, z = -10 },
						rotation = { x = 0, y = 90, z = 0 },
						scale = { x = 1, y = 1, z = 1 },
						transitionTime = 1.8
					},
				}
			}
		))

	_CreateAndAttachComponentToActor(host, a_platform, "RigidBodyPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "PlatformPhysicsComponent",
				collisionShape = "box",
				halfExtent = { x = 8, y = 1, z = 8 },
				physicsBodyType = PhysicsBodyType.STATIC,
				mass = 0.0
			}
		))

	local a_wall = _CreateActor(host, "Actor",
		"WallActor",
		55, 8, 0,
		0, 0, 90,
		1, 1, 1,
		"")

	local wallMaterial = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_albedo.jpg", "albedo")
	_SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_normal.jpg", "normalMap")
	_SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_roughness.jpg", "roughnessMap")
	_SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_metallic.jpg", "metallicMap")
	_SetFloatToMaterial(host, wallMaterial, 1, "uvScale")

	_CreateAndAttachComponentToActor(host, a_wall, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "WallMeshComponent",
				meshName = "cube.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 8, y = 1, z = 8 },
				luaScriptName = "",
				materialProxyId = wallMaterial
			}
		))

	_CreateAndAttachComponentToActor(host, a_wall, "RigidBodyPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "WallPhysicsComponent",
				collisionShape = "box",
				halfExtent = { x = 8, y = 1, z = 8 },
				physicsBodyType = PhysicsBodyType.STATIC,
				mass = 0.0
			}
		))

	-- ***************************HOUSE******************** --
	local a_house = _CreateActor(host, "Actor", "House",
		0, 10, 0,
		0, 0, 0,
		1, 1, 1,
		"")


	local houseMaterial = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, houseMaterial, "city_house_2_Col.jpg", "albedo")
	_SetTextureToMaterial(host, houseMaterial, "city_house_2_Nor.jpg", "normalMap")
	_SetTextureToMaterial(host, houseMaterial, "city_house_2_Spec.png", "roughnessMap")
	_SetTextureToMaterial(host, houseMaterial, "dummy_metallic_roughness.png", "metallicMap")
	_SetFloatToMaterial(host, houseMaterial, 1, "uvScale")

	_CreateAndAttachComponentToActor(host, a_house, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "HosueMeshComponent",
				meshName = "italian_house_1.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 15, y = 15, z = 15 },
				luaScriptName = "",
				materialProxyId = houseMaterial
			}
		))

	_CreateAndAttachComponentToActor(host, a_house, "RigidBodyPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "HousePhysicsComponent",
				collisionShape = "box",
				halfExtent = { x = 7.5, y = 7.5, z = 7.5 },
				physicsBodyType = PhysicsBodyType.DYNAMIC,
				mass = 500.0
			}
		))

	local a_grave = _CreateActor(host, "Actor", "Grave",
		15, 5, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	local graveMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, graveMat, "Brick_Medieval_albedo.jpg", "albedo")
	_SetTextureToMaterial(host, graveMat, "Brick_Medieval_normal.jpg", "normalMap")
	_SetTextureToMaterial(host, graveMat, "Brick_Medieval_roughness.jpg", "roughnessMap")
	_SetTextureToMaterial(host, graveMat, "Brick_Medieval_metallic.jpg", "metallicMap")
	_SetFloatToMaterial(host, graveMat, 5.0, "uvScale")

	_CreateAndAttachComponentToActor(host, a_grave, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "GraveMeshComponent",
				meshName = "witcher.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 1.5, y = 1.5, z = 1.5 },
				luaScriptName = "",
				materialProxyId = graveMat
			}
		))

	_CreateAndAttachComponentToActor(host, a_grave, "RigidBodyPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "HousePhysicsComponent",
				physicsBodyType = PhysicsBodyType.DYNAMIC,
				mass = 1000.0,
				collisionShape = "compoundShape",
				subshapes = {
					leftSphere = {
						collisionShape = "sphere",
						radius = 5,
						translation = { x = -4, y = 0, z = 0 },
						rotation = { x = 0, y = 0, z = 0 }
					},
					rightSphere = {
						collisionShape = "sphere",
						radius = 5,
						translation = { x = 4, y = 0, z = 0 },
						rotation = { x = 0, y = 0, z = 0 }
					}
				}
			}
		))

	-- ***************************SKELET******************** --

	local a_skelet = _CreateActor(host, "Actor",
		"SkeletActor",
		10, 50, 10,
		0, 0, 0,
		1, 1, 1,
		"")

	local skeletMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, skeletMat, "dummy_nm.png", "albedo")
	_SetTextureToMaterial(host, skeletMat, "dummy_nm.png", "normalMap")
	_SetTextureToMaterial(host, skeletMat, "dummy_metallic_roughness.png", "roughnessMap")
	_SetTextureToMaterial(host, skeletMat, "dummy_metallic_roughness.png", "metallicMap")
	_SetFloatToMaterial(host, skeletMat, 1.0, "uvScale")

	_CreateAndAttachComponentToActor(host, a_skelet, "CharacterPhysicsComponent",
		Json.encode(
			{
				gameObjectName = "HousePhysicsComponent",
				capsuleRadius = 2.5,
				capsuleHeight = 10,
				stepHeight = 1.0,
				mass = 500
			}
		))

	_CreateAndAttachComponentToActor(host, a_skelet, "HumanoidPhysicsMovementComponent",
		Json.encode(
			{
				gameObjectName = "SkeletMovementComponent",
				launchDirection = { x = 0, y = 0, z = 0 },
				cameraName = "MainCamera"
			}
		))

	_CreateAndAttachComponentToActor(host, a_skelet, "SkeletalMeshComponent",
		Json.encode(
			{
				gameObjectName = "SkeletMeshComponent",
				meshName = "tina.fbx",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 90 },
				scale = { x = 8, y = 8, z = 8 },
				luaScriptName = "",
				materialProxyId = skeletMat
			}
		))

	_CreateAndAttachComponentToActor(host, a_skelet, "InputComponent",
		Json.encode(
			{
				gameObjectName = "SkeletInputComponent"
			}
		))

	_CreateActorController(host, "DefaultActorControllerCreatorFactory", "SkeletActor",
		"HumanoidPlayerController", Json.encode(
			{
				cameraName = "MainCamera"
			}
		))


	-- local skeletAnimationTweener = _CreateTweener(host, a_skelet, "playerAnimation.tween")
	-- _SetTweenerBinding(host, a_skelet, skeletAnimationTweener, "SkeletMeshComponent", "animationBinding", "")

	-- -- ****************************WATER***************************** --
	local a_water = _CreateActor(host, "Actor",
		"WaterActor",
		20, 2, 0,
		0, 0, 0,
		1, 1, 1,
		"")


	local waterMat = _CreateMaterial(host, "WaterMaterial.m")
	_SetDeferredTextureToMaterial(host, waterMat, "PlanarReflectionComponent", "reflectionTexture")
	_SetTextureToMaterial(host, waterMat, "water_dudv.jpg", "dudv")
	_SetTextureToMaterial(host, waterMat, "brick_mid.jpg", "ground")
	_SetBindingToMaterial(host, waterMat, "EngineScene", "GT_DeltaSec", "deltaTime")
	_SetFloatToMaterial(host, waterMat, 0.5, "mul_coef")

	_CreateAndAttachComponentToActor(host, a_water, "WaterPlaneComponent",
		Json.encode(
			{
				gameObjectName = "WaterMeshComponent",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 20, y = 1, z = 20 },
				materialProxyId = waterMat
			}
		))
end

function System_OnStart(host)
	CreateTestLevel(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
