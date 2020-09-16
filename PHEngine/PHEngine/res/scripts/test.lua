function Create(host)
	
	local actorTranslation = { x = 0, y = 0, z = 0 }
	local actorRotation = { x = 0, y = 0, z = 0 }
	local actorScale = { x = 1, y = 1, z = 1 }

	local ligthActor = _CreateActor(host, "DirectionalLightActor", actorTranslation.x,actorTranslation.y,actorTranslation.z,
		actorRotation.x,actorRotation.y,actorRotation.z,
		actorScale.x, actorScale.y, actorScale.z)

	local rotation = { x = 0, y = 0, z = 0 }
	local direction = { x = -0.5, y = -0.5, z = 0 }
	local ambient = { x = 0.2, y = 0.2, z = 0.2}
	local diffuse = { x = 0.5, y = 0.8 , z = 0.3 }
	local specular = { x = 0.1, y = 0.1, z = 0.1}

	local dirLightComponentData = _CreateDirLightComponentData(host, rotation.x, rotation.y, rotation.z,
		direction.x, direction.y, direction.z,
		ambient.x, ambient.y, ambient.z,
		diffuse.x, diffuse.y, diffuse.z,
		specular.x, specular.y, specular.z
		)

	local dirLightComponentData1 = _CreateDirLightComponentData(host, rotation.x, rotation.y, rotation.z,
		direction.x, direction.y, direction.z,
		ambient.x, ambient.y, ambient.z,
		0.9, 0, 0,
		specular.x, specular.y, specular.z
	)

	local dirLightComponent = _CreateComponent(host, "DirLComponent", dirLightComponentData)
	local dirLightComponent1 = _CreateComponent(host, "DirLComponent", dirLightComponentData1)

	_AttachComponentToActor(host, ligthActor, dirLightComponent)
	_AttachComponentToActor(host, ligthActor, dirLightComponent1)
end