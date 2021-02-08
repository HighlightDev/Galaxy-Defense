function CreateTestLevel(host)
	
	local actorTranslation = { x = 0, y = 0, z = 0 }
	local actorRotation = { x = 0, y = 0, z = 0 }
	local actorScale = { x = 1, y = 1, z = 1 }

	-- ****************************LIGHT***************************** --

	local lightActor = _CreateActor(host, "MainLightActor", actorTranslation.x,actorTranslation.y,actorTranslation.z,
		actorRotation.x,actorRotation.y,actorRotation.z,
		actorScale.x, actorScale.y, actorScale.z)

	if lightActor ~= nil then
		local rotation = { x = 0, y = 0, z = 0 }
		local direction = { x = -0.5, y = -0.5, z = 0 }
		local ambient = { x = 0.2, y = 0.2, z = 0.2}
		local diffuse = { x = 1.68, y = 1.5, z = 1.5 }
		local specular = { x = 0.7, y = 0.7, z = 0.7 }

		 local attenuation = { x = 0, y = 0, z = 0 }
		 local pointLTranslation = {x = 0 , y = 5, z = 0}

		local dirShadowInfo = _CreateLightProjectionShadowInfo(host, 512, "direct_light")
		--local dirShadowInfo1= _CreateLightProjectionShadowInfo(host, 512, "direct_light")
		--local spotlightShadowInfo = _CreateLightProjectionShadowInfo(host, 512, "spotlight")
		--	local spotlightCD = _CreateSpotlightComponentData(host, 
		--	"spotlightComp",
		--	-10, 2, 0,
		--	rotation.x, 0, rotation.z,
		--	ambient.x, ambient.y, ambient.z,
		--	diffuse.x, diffuse.y, diffuse.z,
		--	specular.x, specular.y, specular.z,
		--	attenuation.x, attenuation.y, attenuation.z,
		--	100.0,
		--	0.88,
		--	spotlightShadowInfo)
		--local spotlightComponent = _CreateComponent(host, "SpotlightComponent", spotlightCD)
		--_AttachComponentToActor(host, lightActor, spotlightComponent)

		local dirLightComponentData = _CreateDirLightComponentData(host, "MainLightComp",
			rotation.x, rotation.y, rotation.z,
			direction.x, direction.y, direction.z,
			ambient.x, ambient.y, ambient.z,
			diffuse.x, diffuse.y, diffuse.z,
			specular.x, specular.y, specular.z,
			dirShadowInfo
			)
		--
		--			local dirLightComponentData1 = _CreateDirLightComponentData(host, "MainLightComp1",
		--	rotation.x, rotation.y, rotation.z,
		--	-direction.x, direction.y, direction.z,
		--	ambient.x, ambient.y, ambient.z,
		--	diffuse.x, diffuse.y, diffuse.z,
		--	specular.x, specular.y, specular.z,
		--	dirShadowInfo1
		--	)


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

		 local dirLightComponent = _CreateComponent(host, "DirectionalLightComponent", dirLightComponentData)
		 --local dirLightComponent1 = _CreateComponent(host, "DirectionalLightComponent", dirLightComponentData1)
		-- local pointLightComponent = _CreateComponent(host, "PointLightComponent", pointLightComponentData)

		 _AttachComponentToActor(host, lightActor, dirLightComponent)
		-- _AttachComponentToActor(host, lightActor, dirLightComponent1)
		-- _AttachComponentToActor(host, lightActor, pointLightComponent)
	end

	-- ****************************BIG GROUND***************************** --
	local material = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, material, "brick_mid.png", "albedo")
	_SetTextureToMaterial(host, material, "brick_nm_mid.png", "normalMap")
	_SetFloatToMaterial(host, material, 10.0, "uvScale")

	local groundActor = _CreateActor(host, "Ground",
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)

	local meshData = _CreateMeshComponentData(host, "floor1Comp", "playerCube.obj", 0, 0, 0, 0, 0, 0, 50, 1, 50, "", material)
	local floorComponent = _CreateComponent(host, "StaticMeshComponent", meshData)
	_AttachComponentToActor(host, groundActor, floorComponent)

	local shape = _CreatePhysicsBoxShape(host, 50, 1, 50)
	local floorPhysDesc = _CreateRigidBodyController(host, shape, "STATIC_BODY", 0.0)
	local physData = _CreatePhysicsComponentData(host, "FloorPhysicsComp", floorPhysDesc)
	local phyComponent = _CreateComponent(host, "PhysicsComponent", physData)
	_AttachComponentToActor(host, groundActor, phyComponent)

	-- ****************************SMALL GROUND***************************** --

	local smallGroundActor = _CreateActor(host, "SmallGround",
	0, 15, 0,
	0, 0, 0,
	1, 1, 1)

	local material1 = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, material1, "brick_mid.png", "albedo")
	_SetTextureToMaterial(host, material1, "brick_nm_mid.png", "normalMap")
	_SetFloatToMaterial(host, material1, 1, "uvScale")

	local smallMeshData = _CreateMeshComponentData(host, "playerCubeMeshComp", "playerCube.obj", 0, 0, 0, 0, 0, 0, 8, 1, 8, "", material1)
	local moveCompData = _CreateMovementComponentData(host, "moveCompData", "platformMovementComponentAction.lua")
	local floorComponent = _CreateComponent(host, "StaticMeshComponent", smallMeshData)
	local moveComponent = _CreateComponent(host, "MovementComponent", moveCompData)
	_AttachComponentToActor(host, smallGroundActor, floorComponent)
	_AttachComponentToActor(host, smallGroundActor, moveComponent)

	local shape1 = _CreatePhysicsBoxShape(host, 8, 1, 8)
	local floorPhysDesc1 = _CreateRigidBodyController(host, shape1, "KINEMATIC_BODY", 0.0)
	local physData1 = _CreatePhysicsComponentData(host, "smallFloorPhysComp", floorPhysDesc1)
	local phyComponent1 = _CreateComponent(host, "PhysicsComponent", physData1)
	_AttachComponentToActor(host, smallGroundActor, phyComponent1)


	-- THIS IS A CODE SNIPPET FOR SPOTLIGHT TEST
		local smallGroundActor1 = _CreateActor(host, "SmallGround1",
	5, 3, 0,
	0, 0, 90,
	1, 1, 1)

	local material2 = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, material2, "brick_mid.png", "albedo")
	_SetTextureToMaterial(host, material2, "brick_nm_mid.png", "normalMap")
	_SetFloatToMaterial(host, material2, 1, "uvScale")

	local smallMeshData1 = _CreateMeshComponentData(host, "playerCubeMeshComp1", "playerCube.obj", 0, 0, 0, 0, 0, 0, 8, 1, 8, "", material2)
	local floorComponent1 = _CreateComponent(host, "StaticMeshComponent", smallMeshData1)
	_AttachComponentToActor(host, smallGroundActor1, floorComponent1)

	local shape2 = _CreatePhysicsBoxShape(host, 8, 1, 8)
	local floorPhysDesc2 = _CreateRigidBodyController(host, shape2, "STATIC_BODY", 0.0)
	local physData2 = _CreatePhysicsComponentData(host, "smallFloorPhysComp1", floorPhysDesc2)
	local phyComponent2 = _CreateComponent(host, "PhysicsComponent", physData2)
	_AttachComponentToActor(host, smallGroundActor1, phyComponent2)

	-- ***************************HOUSE******************** --

	local house = _CreateActor(host,"House", 
	0, 5, 0,
	0, 0, 0,
	1, 1, 1)

	local houseMat = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Col.png", "albedo")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Nor.png", "normalMap")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Spec.png", "metallicMap")
	_SetFloatToMaterial(host, houseMat, 1.0, "uvScale")

	local houseData = _CreateMeshComponentData(host, "houseMeshComp", "City_House_2_BI.obj",
	0, -2, 0,
	0, 0, 0,
	0.5, 0.5, 0.5, 
	"",
	houseMat)

	local meshComponent = _CreateComponent(host, "StaticMeshComponent", houseData)
	_AttachComponentToActor(host, house, meshComponent)

	local houseShape = _CreatePhysicsBoxShape(host, 1, 1.5, 1)
	local houseDesc = _CreateRigidBodyController(host, houseShape, "DYNAMIC_BODY", 125.0)
	local housePhysCompData = _CreatePhysicsComponentData(host, "housePhyComp", houseDesc)
	local housePhysComp = _CreateComponent(host, "PhysicsComponent", housePhysCompData)
	_AttachComponentToActor(host, house, housePhysComp)

	-- ***************************SKYBOX******************** --

	local skyboxActor = _CreateActor(host,"Skybox actor", 
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)

	local skyboxMat = _CreateMaterial(host, "Skybox.m")
	_SetTextureToMaterial(host, skyboxMat, "dayRight.png,dayLeft.png,dayTop.png,dayBottom.png,dayBack.png,dayFront.png", "dayTexture")

	local skyboxData = _CreateSkyboxComponentData(host, "SkyboxComp",
	140, 140, 140,
	skyboxMat)

	local skyboxComponent = _CreateComponent(host, "SkyboxComponent", skyboxData)
	_AttachComponentToActor(host, skyboxActor, skyboxComponent)

	-- ***************************SKELET******************** --
	local buddy = _CreateActor(host,"SkeletBuddy", 
	10, 50, 10,
	0, 0, 0,
	1, 1, 1)

	local buddyMat = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "albedo")
	_SetTextureToMaterial(host, buddyMat, "dummy_nm.png", "normalMap")
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

	_AttachComponentToActor(host, buddy, skeletComponent)
	_AttachComponentToActor(host, buddy, skeletPhysComp)
	_AttachComponentToActor(host, buddy, skeletInputComponent)
	_AttachComponentToActor(host, buddy, skeletMovementComponent)

	local buddyAnimationStateMachine = _CreateStateMachine(host, buddy, "playerAnimation.fsm")
	_SetFSMBinding(host, buddyAnimationStateMachine, "buddyMeshComp", "animationBinding", "")

	_AttachPlayerControllerToActor(host, buddy)

end