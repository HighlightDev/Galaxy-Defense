function CreateTestLevel(host)
	
	_LoadResourcesAsync(host, 
	[[dayRight.jpg
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
	,spaceship.obj
	,spaceship_albedo.jpg
	,spaceship_normal.jpg
	,spaceship_roughness.jpg
	,spaceship_metallic.jpg
	]])

    _CreateFirstPersonCamera(host, "MainCamera",
	    0, 0, _GetWindowWidth(host), _GetWindowHeight(host),
	    25.0,
	    20.0,
	    0, 0, -10)

	local a_skybox = _CreateActor(host,"SkyboxActor", 
	0, 0, 0,
	0, 0, 0,
	1, 1, 1)

    if a_skybox ~= nil then
	    local mat = _CreateMaterial(host, "SkyboxMaterial.m")
	    _SetTextureToMaterial(host, mat, "dayRight.jpg,dayLeft.jpg,dayTop.jpg,dayBottom.jpg,dayBack.jpg,dayFront.jpg","dayTexture")
	    _SetTextureToMaterial(host, mat,"nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg",	"nightTexture")

	    _SetBindingToMaterial(host, mat, "EngineScene", "GT_DeltaSec", "deltaTime")
	    _SetFloatToMaterial(host, mat, 0.1, "mul_coef")

	    local d_skybox = _CreateSkyboxComponentData(host, "SkyboxComp",
	    140, 140, 140,
	    mat)

	    local c_skybox = _CreateComponent(host, "SkyboxComponent", d_skybox)
	    _AttachComponentToActor(host, a_skybox, c_skybox)
    end

    local a_spaceship = _CreateActor(host,"SpaceshipActor", 
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
		0, 0, 0,
		9, 9, 9, 
		"",
		mat)

		local c_spaceship = _CreateComponent(host, "StaticMeshComponent", d_spaceship)
		_AttachComponentToActor(host, a_spaceship, c_spaceship)
    end

end

function System_OnStart(host)
	CreateTestLevel(host)
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)