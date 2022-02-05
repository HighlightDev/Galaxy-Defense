function CreateTestLevel(host)
	
	_LoadResourcesAsync(host, 
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
	,playerCube.obj
	,witcher.obj
	,City_House_2_BI.obj
	,model.dae
	,player_walk.fbx
	,spaceship.obj
	,spaceship_albedo.jpg
	,spaceship_normal.jpg
	,spaceship_roughness.jpg
	,spaceship_metallic.jpg
	]])
	 
	local aTra = { x = 0, y = 0, z = 0 }
	local aRot = { x = 0, y = 0, z = 0 }
	local aSca = { x = 1, y = 1, z = 1 }

	--void(std::string, glm::ivec4, float, float, float, glm::vec3, int32_t)
	-- *********************************CREATE MAIN CAMERA******************************** --
	_CreateThirdPersonCamera(host, "MainCamera",
	0, 0, _GetWindowWidth(host), _GetWindowHeight(host),
	50.0,
	20.0,
	20.0,
	0, 5, 0,
	1) -- is main camera on scene


	-- ****************************PLANAR REFLECTION***************************** --

	local planarReflectionCD = _CreatePlanarReflectionComponentData(host, "planarReflectionComponent",
	0, 2, 0,
	0, 0, 0,
	1, 1, 1,
	"MainCamera",
	0, 0, _GetWindowWidth(host), _GetWindowHeight(host))
	_CreateComponent(host, "PlanarReflectionComponent", planarReflectionCD)

	-- ****************************LIGHT***************************** --

	local lightActor = _CreateActor(host, "MainLightActor", aTra.x,aTra.y,aTra.z, aRot.x,aRot.y,aRot.z,aSca.x, aSca.y, aSca.z)
	
	if lightActor ~= nil then
		local rotation = { x = 0, y = 0, z = 0 }
		local direction = { x = -0.5, y = -0.5, z = 0 }
		local ambient = { x = 0.2, y = 0.2, z = 0.2}
		local diffuse = { x = 1.68, y = 1.5, z = 1.5 }
		local specular = { x = 0.7, y = 0.7, z = 0.7 }
		
		local attenuation = { x = 0, y = 0, z = 0 }
		local pointLTranslation = {x = 15 , y = 15, z = 0}
	
		local dirShadowInfo = _CreateLightProjectionShadowInfo(host, 512, "direct_light")
		local dirShadowInfo1= _CreateLightProjectionShadowInfo(host, 256, "direct_light")
	
		local dirLightComponentData = _CreateDirLightComponentData(host, "MainLightComp",
			rotation.x, rotation.y, rotation.z,
			direction.x, direction.y, direction.z,
			ambient.x, ambient.y, ambient.z,
			diffuse.x, diffuse.y, diffuse.z,
			specular.x, specular.y, specular.z,
			dirShadowInfo
		)
		local dirLightComponent = _CreateComponent(host, "DirectionalLightComponent", dirLightComponentData)
		 _AttachComponentToActor(host, "MainLightActor", dirLightComponent)

		--	local dirLightComponentData1 = _CreateDirLightComponentData(host, "MainLightComp1",
		--	rotation.x, rotation.y, rotation.z,
		--	-direction.x, direction.y, direction.z,
		--	ambient.x, ambient.y, ambient.z,
		--	diffuse.x, diffuse.y, diffuse.z,
		--	specular.x, specular.y, specular.z,
		--	dirShadowInfo1
		--	)
		-- local dirLightComponent1 = _CreateComponent(host, "DirectionalLightComponent", dirLightComponentData1)
		--_AttachComponentToActor(host, "MainLightActor", dirLightComponent1)


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
	end

	-- ****************************BIG GROUND***************************** --
	
	local groundActor = _CreateActor(host, "Ground",
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)
	
	if groundActor ~= nil then
		local material = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, material, "brick_mid.jpg", "albedo")
		_SetTextureToMaterial(host, material, "brick_nm_mid.jpg", "normalMap")
		_SetTextureToMaterial(host, material, "dummy_metallic_roughness.png", "roughnessMap")
		_SetTextureToMaterial(host, material, "dummy_metallic_roughness.png", "metallicMap")
		_SetFloatToMaterial(host, material, 10.0, "uvScale")

		local meshData = _CreateMeshComponentData(host, "floor1Comp", "playerCube.obj", 0, 0, 0, 0, 0, 0, 50, 1, 50, "", material)
		local floorComponent = _CreateComponent(host, "StaticMeshComponent", meshData)
		_AttachComponentToActor(host, "Ground", floorComponent)

		local shape = _CreatePhysicsBoxShape(host, 50, 1, 50)
		local floorPhysDesc = _CreateRigidBodyController(host, shape, "STATIC_BODY", 0.0)
		local physData = _CreatePhysicsComponentData(host, "FloorPhysicsComp", floorPhysDesc)
		local phyComponent = _CreateComponent(host, "PhysicsComponent", physData)
		_AttachComponentToActor(host, "Ground", phyComponent)
	end

	-- ****************************SMALL GROUND***************************** --

	local smallGroundActor = _CreateActor(host, "SmallGround",
	0, 10, 0,
	0, 0, 0,
	1, 1, 1)

	if smallGroundActor ~= nil then 
		local material1 = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, material1, "brick_mid.jpg", "albedo")
		_SetTextureToMaterial(host, material1, "brick_nm_mid.jpg", "normalMap")
		_SetTextureToMaterial(host, material1, "dummy_metallic_roughness.png", "roughnessMap")
		_SetTextureToMaterial(host, material1, "dummy_metallic_roughness.png", "metallicMap")
		_SetFloatToMaterial(host, material1, 1, "uvScale")

		local smallMeshData = _CreateMeshComponentData(host, "playerCubeMeshComp", "playerCube.obj", 0, 0, 0, 0, 0, 0, 8, 1, 8, "", material1)
		local moveCompData = _CreatePlatformMovementComponentData(host, "moveCompData", "platformMovementComponentAction.lua")
		local floorComponent = _CreateComponent(host, "StaticMeshComponent", smallMeshData)
		local moveComponent = _CreateComponent(host, "PlatformMovementComponent", moveCompData)
		_AttachComponentToActor(host, "SmallGround", floorComponent)
		_AttachComponentToActor(host, "SmallGround", moveComponent)

		local shape1 = _CreatePhysicsBoxShape(host, 8, 1, 8)
		local floorPhysDesc1 = _CreateRigidBodyController(host, shape1, "KINEMATIC_BODY", 0.0)
		local physData1 = _CreatePhysicsComponentData(host, "smallFloorPhysComp", floorPhysDesc1)
		local phyComponent1 = _CreateComponent(host, "PhysicsComponent", physData1)
		_AttachComponentToActor(host, "SmallGround", phyComponent1)
	end

	-- THIS IS A CODE SNIPPET FOR SPOTLIGHT TEST
	local smallGroundActor1 = _CreateActor(host, "SmallGround1",
	5, 8, 0,
	0, 0, 90,
	1, 1, 1)

	if smallGroundActor1 ~= nil then 
		local material2 = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, material2, "Brick_Medieval_albedo.jpg", "albedo")
		_SetTextureToMaterial(host, material2, "Brick_Medieval_normal.jpg", "normalMap")
		_SetTextureToMaterial(host, material2, "Brick_Medieval_roughness.jpg", "roughnessMap")
		_SetTextureToMaterial(host, material2, "Brick_Medieval_metallic.jpg", "metallicMap")
		_SetFloatToMaterial(host, material2, 1, "uvScale")

		local smallMeshData1 = _CreateMeshComponentData(host, "playerCubeMeshComp1", "playerCube.obj", 0, 0, 0, 0, 0, 0, 8, 1, 8, "", material2)
		local floorComponent1 = _CreateComponent(host, "StaticMeshComponent", smallMeshData1)
		_AttachComponentToActor(host, "SmallGround1", floorComponent1)

		local shape2 = _CreatePhysicsBoxShape(host, 8, 1, 8)
		local floorPhysDesc2 = _CreateRigidBodyController(host, shape2, "STATIC_BODY", 0.0)
		local physData2 = _CreatePhysicsComponentData(host, "smallFloorPhysComp1", floorPhysDesc2)
		local phyComponent2 = _CreateComponent(host, "PhysicsComponent", physData2)
		_AttachComponentToActor(host, "SmallGround1", phyComponent2)
	end

	-- ***************************HOUSE******************** --

	local house = _CreateActor(host, "House", 
	5, 15, 0,
	0, 0, 0,
	1, 1, 1)

	if house ~= nil then 
		local houseMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, houseMat, "spaceship_albedo.jpg", "albedo")
		_SetTextureToMaterial(host, houseMat, "spaceship_normal.jpg", "normalMap")
		_SetTextureToMaterial(host, houseMat, "spaceship_roughness.jpg", "roughnessMap")
		_SetTextureToMaterial(host, houseMat, "spaceship_metallic.jpg", "metallicMap")
		_SetFloatToMaterial(host, houseMat, 1.0, "uvScale")

		local houseData = _CreateMeshComponentData(host, "houseMeshComp", "spaceship.obj",
		0, 0, 0,
		0, 0, 0,
		4.5, 4.5, 4.5, 
		"",
		houseMat)

		local meshComponent = _CreateComponent(host, "StaticMeshComponent", houseData)
		_AttachComponentToActor(host, "House", meshComponent)

		local houseShape = _CreatePhysicsBoxShape(host, 3, 4.5, 3)
		local houseDesc = _CreateRigidBodyController(host, houseShape, "DYNAMIC_BODY", 525.0)
		local housePhysCompData = _CreatePhysicsComponentData(host, "housePhyComp", houseDesc)
		local housePhysComp = _CreateComponent(host, "PhysicsComponent", housePhysCompData)
		_AttachComponentToActor(host, "House", housePhysComp)
	end

	-- ***************************TEST******************** --

	local test = _CreateActor(host, "test", 
	15, 5, 0,
	0, 0, 0,
	1, 1, 1)

	if test ~= nil then
		local testMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
		_SetTextureToMaterial(host, testMat, "Brick_Medieval_albedo.jpg", "albedo")
		_SetTextureToMaterial(host, testMat, "Brick_Medieval_normal.jpg", "normalMap")
		_SetTextureToMaterial(host, testMat, "Brick_Medieval_roughness.jpg", "roughnessMap")
		_SetTextureToMaterial(host, testMat, "Brick_Medieval_metallic.jpg", "metallicMap")
		_SetFloatToMaterial(host, testMat, 5.0, "uvScale")

		local testData = _CreateMeshComponentData(host, "testMeshComponent", "witcher.obj",
		0, 0, 0,
		0, 0, 0,
		1.5, 1.5, 1.5, 
		"",
		testMat)

		local testMeshComponent = _CreateComponent(host, "StaticMeshComponent", testData)
		_AttachComponentToActor(host, "test", testMeshComponent)

		local compoundShape = _CreatePhysicsCompoundShape(host)
		_AddCompoundChildShape(host, compoundShape, _CreatePhysicsSphereShape(host, 5), -4, 0, 0, 0 ,0 ,0)
		_AddCompoundChildShape(host, compoundShape, _CreatePhysicsSphereShape(host, 5), 4, 0, 0, 0 ,0 ,0)
		local testDesc = _CreateRigidBodyController(host, compoundShape, "DYNAMIC_BODY", 1000.0)
		local testCompData = _CreatePhysicsComponentData(host, "testPhyComp", testDesc)
		_AttachComponentToActor(host, "test",  _CreateComponent(host, "PhysicsComponent", testCompData))
	end

	-- ***************************SKYBOX******************** --

	local skyboxActor = _CreateActor(host, "Skybox actor", 
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)

	local skyboxMat = _CreateMaterial(host, "SkyboxMaterial.m")
	_SetTextureToMaterial(host, skyboxMat, "dayRight.jpg,dayLeft.jpg,dayTop.jpg,dayBottom.jpg,dayBack.jpg,dayFront.jpg","dayTexture")

	_SetTextureToMaterial(host, skyboxMat,"nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg",	"nightTexture")

	_SetBindingToMaterial(host, skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTime")
	_SetFloatToMaterial(host, skyboxMat, 0.1, "mul_coef")

	local skyboxData = _CreateSkyboxComponentData(host, "SkyboxComp",
	140, 140, 140,
	skyboxMat)

	local skyboxComponent = _CreateComponent(host, "SkyboxComponent", skyboxData)
	_AttachComponentToActor(host, "Skybox actor", skyboxComponent)

	-- ***************************SKELET******************** --
	local buddy = _CreateActor(host, "SkeletBuddy", 
	10, 50, 10,
	0, 0, 0,
	1, 1, 1)

	local buddyMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
	_SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "albedo")
	_SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "normalMap")
	_SetTextureToMaterial(host, buddyMat, "dummy_metallic_roughness.png", "roughnessMap")
	_SetTextureToMaterial(host, buddyMat, "dummy_metallic_roughness.png", "metallicMap")
	_SetFloatToMaterial(host, buddyMat, 1.0, "uvScale")

	local skeletDesc = _CreateDynamicCharacterController(host, 1, 2.5, 10, 1.0)
	local skeletPhysCompData = _CreatePhysicsComponentData(host, "buddyPhyComp", skeletDesc)
	local skeletInputCompData = _CreateInputComponentData(host, "skeletInputComp")
	local skeletMovementCompData = _CreateCharacterMovementComponentData(host, "charMovementCompData", 0, 0, 0, "MainCamera")
	local buddyData = _CreateMeshComponentData(host, "buddyMeshComp", "player_walk.fbx",
	0, -0.6, 0,
	0, 0, 0,
	3, 3, 3, 
	"skeletComponentAction.lua",
	buddyMat)

	local skeletPhysComp = _CreateComponent(host, "CharacterPhysicsComponent", skeletPhysCompData)
	local skeletInputComponent = _CreateComponent(host, "InputComponent", skeletInputCompData)
	local skeletMovementComponent = _CreateComponent(host, "CharacterMovementComponent", skeletMovementCompData)
	local skeletComponent = _CreateComponent(host, "SkeletalMeshComponent", buddyData)

	_AttachComponentToActor(host, "SkeletBuddy", skeletComponent)
	_AttachComponentToActor(host, "SkeletBuddy", skeletPhysComp)
	_AttachComponentToActor(host, "SkeletBuddy", skeletInputComponent)
	_AttachComponentToActor(host, "SkeletBuddy", skeletMovementComponent)

	local buddyAnimationTweener = _CreateTweener(host, buddy, "playerAnimation.tween")
	_SetTweenerBinding(host, buddyAnimationTweener, "buddyMeshComp", "animationBinding", "")	

	_AttachPlayerControllerToActor(host, buddy)

	-- ****************************WATER***************************** --
	local waterActor = _CreateActor(host, "WaterActor",
	20, 2, 0,
	0, 0, 0,
	1, 1, 1)

	if waterActor ~= nil then
		local waterMat = _CreateMaterial(host, "WaterMaterial.m")
		_SetDeferredTextureToMaterial(host, waterMat, "planarReflectionComponent", "reflectionTexture")
		_SetTextureToMaterial(host, waterMat, "water_dudv.jpg", "dudv")
		_SetTextureToMaterial(host, waterMat, "brick_mid.jpg", "ground")
		_SetBindingToMaterial(host, waterMat, "EngineScene", "GT_DeltaSec", "deltaTime")
		_SetFloatToMaterial(host, waterMat, 0.5, "mul_coef")

		local waterMeshData = 	_CreateWaterPlaneComponentData(host, "waterComponent", 0, 0, 0, 0, 0, 0, 20, 1, 20, waterMat)
		local waterComponent = _CreateComponent(host, "WaterPlaneComponent", waterMeshData)
		_AttachComponentToActor(host, "WaterActor", waterComponent)
	end
end

function System_OnStart(host)
	CreateTestLevel(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)