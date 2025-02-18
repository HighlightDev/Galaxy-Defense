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
	,City_House_2_BI.obj
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
	,nimbus_mono.png
	,arrow_right_1.png
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

	_CreateActor(host, "Actor",
		"SceneCenterActorDummy",
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		"")

	_SetCameraThirdPersonTarget(host, "MainCamera", "SceneCenterActorDummy")

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
				is_enabled = true,
				is_visible = true,
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
	_SetFloatToMaterial(host, groundMat, 1.0, "uvScale")

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

	_CreateAndAttachComponentToActor(host, a_platform, "StaticMeshComponent",
		Json.encode(
			{
				gameObjectName = "PlatformdMeshComponent",
				meshName = "cube.obj",
				translation = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
				scale = { x = 8, y = 1, z = 8 },
				luaScriptName = "",
				materialProxyId = groundMat
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

	-- -- OBSOLETE
	-- local smallGroundActor = _CreateActor(host, "SmallGround",
	-- 	0, 10, 0,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- if smallGroundActor ~= nil then
	-- 	local material1 = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	-- 	_SetTextureToMaterial(host, material1, "brick_mid.jpg", "albedo")
	-- 	_SetTextureToMaterial(host, material1, "brick_nm_mid.jpg", "normalMap")
	-- 	_SetTextureToMaterial(host, material1, "dummy_metallic_roughness.png", "roughnessMap")
	-- 	_SetTextureToMaterial(host, material1, "dummy_metallic_roughness.png", "metallicMap")
	-- 	_SetFloatToMaterial(host, material1, 1, "uvScale")

	-- 	local smallMeshData = _CreateMeshComponentData(host, "playerCubeMeshComp", "playerCube.obj", 0, 0, 0, 0, 0, 0, 8,
	-- 		1, 8, "", material1)
	-- 	local moveCompData = _CreatePlatformTraverseComponentData(host, "moveCompData",
	-- 		"platformMovementComponentAction.lua")
	-- 	local floorComponent = _CreateComponent(host, "StaticMeshComponent", smallMeshData)
	-- 	local moveComponent = _CreateComponent(host, "PlatformTraverseComponent", moveCompData)
	-- 	_AttachComponentToActor(host, "SmallGround", floorComponent)
	-- 	_AttachComponentToActor(host, "SmallGround", moveComponent)

	-- 	local shape1 = _CreatePhysicsBoxShape(host, 8, 1, 8)
	-- 	local floorPhysDesc1 = _CreateRigidBodyController(host, shape1, "KINEMATIC_BODY", 0.0)
	-- 	local physData1 = _CreatePhysicsComponentData(host, "smallFloorPhysComp", floorPhysDesc1)
	-- 	local phyComponent1 = _CreateComponent(host, "RigidBodyPhysicsComponent", physData1)
	-- 	_AttachComponentToActor(host, "SmallGround", phyComponent1)
	-- end

	-- -- THIS IS A CODE SNIPPET FOR SPOTLIGHT TEST
	-- -- OBSOLETE
	-- local smallGroundActor1 = _CreateActor(host, "SmallGround1",
	-- 	5, 8, 0,
	-- 	0, 0, 90,
	-- 	1, 1, 1)

	-- if smallGroundActor1 ~= nil then
	-- 	local material2 = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	-- 	_SetTextureToMaterial(host, material2, "Brick_Medieval_albedo.jpg", "albedo")
	-- 	_SetTextureToMaterial(host, material2, "Brick_Medieval_normal.jpg", "normalMap")
	-- 	_SetTextureToMaterial(host, material2, "Brick_Medieval_roughness.jpg", "roughnessMap")
	-- 	_SetTextureToMaterial(host, material2, "Brick_Medieval_metallic.jpg", "metallicMap")
	-- 	_SetFloatToMaterial(host, material2, 1, "uvScale")

	-- 	local smallMeshData1 = _CreateMeshComponentData(host, "playerCubeMeshComp1", "playerCube.obj", 0, 0, 0, 0, 0, 0,
	-- 		8, 1, 8, "", material2)
	-- 	local floorComponent1 = _CreateComponent(host, "StaticMeshComponent", smallMeshData1)
	-- 	_AttachComponentToActor(host, "SmallGround1", floorComponent1)

	-- 	local shape2 = _CreatePhysicsBoxShape(host, 8, 1, 8)
	-- 	local floorPhysDesc2 = _CreateRigidBodyController(host, shape2, "STATIC_BODY", 0.0)
	-- 	local physData2 = _CreatePhysicsComponentData(host, "smallFloorPhysComp1", floorPhysDesc2)
	-- 	local phyComponent2 = _CreateComponent(host, "RigidBodyPhysicsComponent", physData2)
	-- 	_AttachComponentToActor(host, "SmallGround1", phyComponent2)
	-- end

	-- -- ***************************HOUSE******************** --
	-- -- OBSOLETE
	-- local house = _CreateActor(host, "House",
	-- 	5, 15, 0,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- if house ~= nil then
	-- 	local houseMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	-- 	_SetTextureToMaterial(host, houseMat, "spaceship_albedo.jpg", "albedo")
	-- 	_SetTextureToMaterial(host, houseMat, "spaceship_normal.jpg", "normalMap")
	-- 	_SetTextureToMaterial(host, houseMat, "spaceship_roughness.jpg", "roughnessMap")
	-- 	_SetTextureToMaterial(host, houseMat, "spaceship_metallic.jpg", "metallicMap")
	-- 	_SetFloatToMaterial(host, houseMat, 1.0, "uvScale")

	-- 	local houseData = _CreateMeshComponentData(host, "houseMeshComp", "spaceship.obj",
	-- 		0, 0, 0,
	-- 		0, 0, 0,
	-- 		4.5, 4.5, 4.5,
	-- 		"",
	-- 		houseMat)

	-- 	local meshComponent = _CreateComponent(host, "StaticMeshComponent", houseData)
	-- 	_AttachComponentToActor(host, "House", meshComponent)

	-- 	local houseShape = _CreatePhysicsBoxShape(host, 3, 4.5, 3)
	-- 	local houseDesc = _CreateRigidBodyController(host, houseShape, "DYNAMIC_BODY", 525.0)
	-- 	local housePhysCompData = _CreatePhysicsComponentData(host, "housePhyComp", houseDesc)
	-- 	local housePhysComp = _CreateComponent(host, "RigidBodyPhysicsComponent", housePhysCompData)
	-- 	_AttachComponentToActor(host, "House", housePhysComp)
	-- end

	-- -- ***************************TEST******************** --
	-- -- OBSOLETE
	-- local test = _CreateActor(host, "test",
	-- 	15, 5, 0,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- if test ~= nil then
	-- 	local testMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	-- 	_SetTextureToMaterial(host, testMat, "Brick_Medieval_albedo.jpg", "albedo")
	-- 	_SetTextureToMaterial(host, testMat, "Brick_Medieval_normal.jpg", "normalMap")
	-- 	_SetTextureToMaterial(host, testMat, "Brick_Medieval_roughness.jpg", "roughnessMap")
	-- 	_SetTextureToMaterial(host, testMat, "Brick_Medieval_metallic.jpg", "metallicMap")
	-- 	_SetFloatToMaterial(host, testMat, 5.0, "uvScale")

	-- 	local testData = _CreateMeshComponentData(host, "testMeshComponent", "witcher.obj",
	-- 		0, 0, 0,
	-- 		0, 0, 0,
	-- 		1.5, 1.5, 1.5,
	-- 		"",
	-- 		testMat)

	-- 	local testMeshComponent = _CreateComponent(host, "StaticMeshComponent", testData)
	-- 	_AttachComponentToActor(host, "test", testMeshComponent)

	-- 	local compoundShape = _CreatePhysicsCompoundShape(host)
	-- 	_AddCompoundChildShape(host, compoundShape, _CreatePhysicsSphereShape(host, 5), -4, 0, 0, 0, 0, 0)
	-- 	_AddCompoundChildShape(host, compoundShape, _CreatePhysicsSphereShape(host, 5), 4, 0, 0, 0, 0, 0)
	-- 	local testDesc = _CreateRigidBodyController(host, compoundShape, "DYNAMIC_BODY", 1000.0)
	-- 	local testCompData = _CreatePhysicsComponentData(host, "testPhyComp", testDesc)
	-- 	_AttachComponentToActor(host, "test", _CreateComponent(host, "RigidBodyPhysicsComponent", testCompData))
	-- end

	-- -- ***************************SKYBOX******************** --
	-- -- OBSOLETE
	-- local skyboxActor = _CreateActor(host, "Skybox actor",
	-- 	0, 0, 0,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- local skyboxMat = _CreateMaterial(host, "SkyboxMaterial.m")
	-- _SetTextureToMaterial(host, skyboxMat, "dayRight.jpg,dayLeft.jpg,dayTop.jpg,dayBottom.jpg,dayBack.jpg,dayFront.jpg",
	-- 	"dayTexture")

	-- _SetTextureToMaterial(host, skyboxMat,
	-- 	"nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg", "nightTexture")

	-- _SetBindingToMaterial(host, skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTime")
	-- _SetFloatToMaterial(host, skyboxMat, 0.1, "mul_coef")

	-- local skyboxData = _CreateSkyboxComponentData(host, "SkyboxComp",
	-- 	140, 140, 140,
	-- 	skyboxMat)

	-- local skyboxComponent = _CreateComponent(host, "SkyboxComponent", skyboxData)
	-- _AttachComponentToActor(host, "Skybox actor", skyboxComponent)

	-- -- ***************************SKELET******************** --
	-- -- OBSOLETE
	-- local buddy = _CreateActor(host, "SkeletBuddy",
	-- 	10, 50, 10,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- local buddyMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	-- _SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "albedo")
	-- _SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "normalMap")
	-- _SetTextureToMaterial(host, buddyMat, "dummy_metallic_roughness.png", "roughnessMap")
	-- _SetTextureToMaterial(host, buddyMat, "dummy_metallic_roughness.png", "metallicMap")
	-- _SetFloatToMaterial(host, buddyMat, 1.0, "uvScale")

	-- local skeletDesc = _CreateDynamicCharacterController(host, 1, 2.5, 10, 1.0)
	-- local skeletPhysCompData = _CreatePhysicsComponentData(host, "buddyPhyComp", skeletDesc)
	-- local skeletInputCompData = _CreateInputComponentData(host, "skeletInputComp")
	-- local skeletMovementCompData = _CreateCharacterMovementComponentData(host, "charMovementCompData", 0, 0, 0,
	-- 	"MainCamera")
	-- local buddyData = _CreateMeshComponentData(host, "buddyMeshComp", "player_walk.fbx",
	-- 	0, -0.6, 0,
	-- 	0, 0, 0,
	-- 	3, 3, 3,
	-- 	"skeletComponentAction.lua",
	-- 	buddyMat)

	-- local skeletPhysComp = _CreateComponent(host, "CharacterPhysicsComponent", skeletPhysCompData)
	-- local skeletInputComponent = _CreateComponent(host, "InputComponent", skeletInputCompData)
	-- local skeletMovementComponent = _CreateComponent(host, "HumanoidPhysicsMovementComponent", skeletMovementCompData)
	-- local skeletComponent = _CreateComponent(host, "SkeletalMeshComponent", buddyData)

	-- _AttachComponentToActor(host, "SkeletBuddy", skeletComponent)
	-- _AttachComponentToActor(host, "SkeletBuddy", skeletPhysComp)
	-- _AttachComponentToActor(host, "SkeletBuddy", skeletInputComponent)
	-- _AttachComponentToActor(host, "SkeletBuddy", skeletMovementComponent)

	-- local buddyAnimationTweener = _CreateTweener(host, buddy, "playerAnimation.tween")
	-- _SetTweenerBinding(host, buddyAnimationTweener, "buddyMeshComp", "animationBinding", "")

	-- _AttachPlayerControllerToActor(host, buddy)

	-- -- ****************************WATER***************************** --
	-- -- OBSOLETE
	-- local waterActor = _CreateActor(host, "WaterActor",
	-- 	20, 2, 0,
	-- 	0, 0, 0,
	-- 	1, 1, 1)

	-- if waterActor ~= nil then
	-- 	local waterMat = _CreateMaterial(host, "WaterMaterial.m")
	-- 	_SetDeferredTextureToMaterial(host, waterMat, "planarReflectionComponent", "reflectionTexture")
	-- 	_SetTextureToMaterial(host, waterMat, "water_dudv.jpg", "dudv")
	-- 	_SetTextureToMaterial(host, waterMat, "brick_mid.jpg", "ground")
	-- 	_SetBindingToMaterial(host, waterMat, "EngineScene", "GT_DeltaSec", "deltaTime")
	-- 	_SetFloatToMaterial(host, waterMat, 0.5, "mul_coef")

	-- 	local waterMeshData = _CreateWaterPlaneComponentData(host, "waterComponent", 0, 0, 0, 0, 0, 0, 20, 1, 20,
	-- 		waterMat)
	-- 	local waterComponent = _CreateComponent(host, "WaterPlaneComponent", waterMeshData)
	-- 	_AttachComponentToActor(host, "WaterActor", waterComponent)
	-- end
end

function System_OnStart(host)
	CreateTestLevel(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
