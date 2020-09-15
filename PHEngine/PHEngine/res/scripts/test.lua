function Create(host, name)
	
	local translation = { x = 10, y = 25, z = 15 }
	local rotation = { x = 0.5, y = 45, z = 15 }
	local scale = { x = 1, y = 1, z = 1}

	local rootComponent = _CreateSceneComponent(host,translation.x, translation.y, translation.z, rotation.x, rotation.y, translation.z, scale.x, scale.y, scale.z)

	if rootComponent ~= nil then
		_GetSceneComponent(host, rootComponent)
	else 
		print("rootComponent is nil!")
	end
end