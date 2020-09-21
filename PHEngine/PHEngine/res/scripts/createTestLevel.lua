function CreateTestLevel(host)
	
	local actorTranslation = { x = 0, y = 0, z = 0 }
	local actorRotation = { x = 0, y = 0, z = 0 }
	local actorScale = { x = 1, y = 1, z = 1 }

	local lightActor = _CreateActor(host, "MainLightActor", actorTranslation.x,actorTranslation.y,actorTranslation.z,
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
	_SetFloatToMaterial(host, material, 10.0, "uvScale")
end