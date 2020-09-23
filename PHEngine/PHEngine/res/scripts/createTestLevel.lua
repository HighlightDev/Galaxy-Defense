function CreateTestLevel(host)
	
	local actorTranslation = { x = 0, y = 0, z = 0 }
	local actorRotation = { x = 0, y = 0, z = 0 }
	local actorScale = { x = 1, y = 1, z = 1 }

	local lightActor = _CreateActor(host, "Actor", "MainLightActor", actorTranslation.x,actorTranslation.y,actorTranslation.z,
		actorRotation.x,actorRotation.y,actorRotation.z,
		actorScale.x, actorScale.y, actorScale.z)

	if lightActor ~= nil then
		local rotation = { x = 0, y = 0, z = 0 }
		local direction = { x = -0.5, y = -0.5, z = 0 }
		local ambient = { x = 0.2, y = 0.2, z = 0.2}
		local diffuse = { x = 1.68, y = 1.5, z = 1.5 }
		local specular = { x = 0.7, y = 0.7, z = 0.7}

		local shadowInfo = _CreateDirLightProjectedShadowInfo(host, 512)

		local dirLightComponentData = _CreateDirLightComponentData(host, rotation.x, rotation.y, rotation.z,
			direction.x, direction.y, direction.z,
			ambient.x, ambient.y, ambient.z,
			diffuse.x, diffuse.y, diffuse.z,
			specular.x, specular.y, specular.z,
			shadowInfo
			)

		local dirLightComponent = _CreateComponent(host, "DirLightComponent", dirLightComponentData)

		_AttachComponentToActor(host, lightActor, dirLightComponent)
	end

	local material = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, material, "brick_mid.png", "albedo")
	_SetTextureToMaterial(host, material, "brick_nm_mid.png", "normalMap")
	_SetFloatToMaterial(host, material, 10.0, "uvScale")

	local groundActor = _CreateActor(host, "Actor", "Ground", 
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)

	local meshData = _CreateMeshComponentData(host, "playerCube.obj", 0, 0, 0, 0, 0, 0, 50, 1, 50, material)
	local floorComponent = _CreateComponent(host, "StaticMeshComponent", meshData)
	_AttachComponentToActor(host, groundActor, floorComponent)

	local shape = _CreatePhysicsBoxShape(host, 50, 1, 50)
	local floorPhysDesc = _CreateRigidBodyController(host, shape, 0.0)
	local physData = _CreatePhysicsComponentData(host, floorPhysDesc)
	local phyComponent = _CreateComponent(host, "PhysicsComponent", physData)
	_AttachComponentToActor(host, groundActor, phyComponent)

	-- ****************************SMALL GROUND***************************** --

	local smallGroundActor = _CreateActor(host, "Actor", "SmallGround", 
	0, 10, 0,
	0, 0, 0,
	1, 1, 1)

	local material1 = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, material1, "brick_mid.png", "albedo")
	_SetTextureToMaterial(host, material1, "brick_nm_mid.png", "normalMap")
	_SetFloatToMaterial(host, material1, 1, "uvScale")

	local smallMeshData = _CreateMeshComponentData(host, "playerCube.obj", 0, 0, 0, 0, 0, 0, 8, 1, 8, material1)
	local floorComponent = _CreateComponent(host, "StaticMeshComponent", smallMeshData)
	_AttachComponentToActor(host, smallGroundActor, floorComponent)

	local shape1 = _CreatePhysicsBoxShape(host, 8, 1, 8)
	local floorPhysDesc1 = _CreateRigidBodyController(host, shape1, 0.0)
	local physData1 = _CreatePhysicsComponentData(host, floorPhysDesc1)
	local phyComponent1 = _CreateComponent(host, "PhysicsComponent", physData1)
	_AttachComponentToActor(host, smallGroundActor, phyComponent1)


	-- ***************************HOUSE******************** --

	local house = _CreateActor(host, "Actor", "House", 
	0, 55, 0,
	0, 0, 0,
	1, 1, 1)

	local houseMat = _CreateMaterial(host, "Pbs.m")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Col.png", "albedo")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Nor.png", "normalMap")
	_SetTextureToMaterial(host, houseMat, "city_house_2_Spec.png", "metallicMap")
	_SetFloatToMaterial(host, houseMat, 1.0, "uvScale")

	local houseData = _CreateMeshComponentData(host, "City_House_2_BI.obj",
	0, -2.5, 0,
	0, 0, 0,
	2.5, 2.5, 2.5, 
	houseMat)

	local meshComponent = _CreateComponent(host, "StaticMeshComponent", houseData)
	_AttachComponentToActor(host, house, meshComponent)

	local houseShape = _CreatePhysicsBoxShape(host, 6, 6.5, 6)
	local houseDesc = _CreateRigidBodyController(host, houseShape, 125.0)
	local housePhysCompData = _CreatePhysicsComponentData(host, houseDesc)
	local housePhysComp = _CreateComponent(host, "PhysicsComponent", housePhysCompData)
	_AttachComponentToActor(host, house, housePhysComp)

end