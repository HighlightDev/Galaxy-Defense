--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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

local PhysicsBodyType = {STATIC = 0, DYNAMIC = 1, KINEMATIC = 2, GHOST = 3}

function CreateTestLevel(host)
    _LazyLoadResourcesAsync(host, [[brick_mid.jpg
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
    ,perlin_noise_128x128.png
    ,plane.obj
	]])

    _OpenAudioStreams(host, [[piano-loop2.ogg
        ]])

    _CreateFirstPersonCamera(host, "MainCamera", 0, 0, _GetWindowWidth(host), _GetWindowHeight(host), Json.encode({
        projectionType = "Perspective",
        FoV = math.rad(60.0),
        AspectRatio = 16.0 / 9.0,
        NearPlane = 1.0,
        FarPlane = 500.0
    }), -- projectionInfo
    40.0, -- pitch
    0.0, -- yaw
    0, 50, -20, 1)

    local beamMat = _CreateMaterial(host, "ElectroBeamMaterial.m")
    _SetTextureToMaterial(host, beamMat, "perlin_noise_128x128.png", "noise")
    _SetBindingToMaterial(host, beamMat, "EngineScene", "GT_DeltaSec", "gt_timeSec")
    _SetVec3ToMaterial(host, beamMat, Json.encode({x = 0.2, y = 1.0, z = 1.0}), "beamGlowColor")
    _SetVec3ToMaterial(host, beamMat, Json.encode({x = 0.6, y = 0.4, z = 1.0}), "beamMainColor")

    local actorId = _CreateActor(host, "Actor", "ElectricBarrier", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")
    _CreateAndAttachComponentToActor(host, actorId, "ElectricBeamComponent", Json.encode({
        gameObjectName = "ElectricBeam",
        startPoint = {x = 10, y = 40, z = 0},
        endPoint = {x = -10, y = 40, z = 0},
        beamThickness = 0.15,
        beamCount = 3,
        jitterAmount = 0.3,
        updateFrequency = 0.03,
        materialProxyId = beamMat
    }))

    -- *********************************CREATE MAIN CAMERA******************************** --
    -- _CreateThirdPersonCamera(host, "MainCamera", 0, 0, _GetWindowWidth(host),
    --                          _GetWindowHeight(host), Json.encode({
    --     projectionType = "Perspective",
    --     FoV = math.rad(60.0),
    --     AspectRatio = 16.0 / 9.0,
    --     NearPlane = 1.0,
    --     FarPlane = 500.0
    -- }), -- projectionInfo
    -- 50.0, 20.0, 220.0, 0, 0, 0, 1) -- is main camera on scene

    -- _SetCameraThirdPersonTarget(host, "MainCamera", "SkeletActor")

    -- ****************************SKYBOX***************************** --
    local a_spaceSkyboxId = _CreateActor(host, "Actor", "SkyboxActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    local skyboxMat = _CreateMaterial(host, "SkyboxMaterial.m")
    _SetTextureToMaterial(host, skyboxMat, "dayRight.jpg,dayLeft.jpg,dayTop.jpg,dayBottom.jpg,dayBack.jpg,dayFront.jpg",
                          "dayTexture")
    _SetTextureToMaterial(host, skyboxMat,
                          "nightRight.jpg,nightLeft.jpg,nightTop.jpg,nightBottom.jpg,nightBack.jpg,nightFront.jpg",
                          "nightTexture")
    _SetBindingToMaterial(host, skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTimeSec")
    _SetFloatToMaterial(host, skyboxMat, 0.01, "mul_coef")

    _CreateAndAttachComponentToActor(host, a_spaceSkyboxId, "SkyboxComponent", Json.encode({
        gameObjectName = "SpaceSkyboxComponent",
        scale = {x = 250, y = 250, z = 250},
        is_visible = true,
        materialProxyId = skyboxMat
    }))

    -- ****************************PLANAR REFLECTION***************************** --

    _CreatePlanarReflectionComponent(host, Json.encode({
        gameObjectName = "PlanarReflectionComponent",
        cameraName = "MainCamera",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 1, y = 1, z = 1},
        viewPortX = 0,
        viewPortY = 0,
        viewPortWidth = _GetWindowWidth(host),
        viewPortHeight = _GetWindowHeight(host)
    }))

    -- -- ****************************LIGHT***************************** --

    local a_dirLightId = _CreateActor(host, "Actor", "DirectionalLightActor", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    _CreateAndAttachComponentToActor(host, a_dirLightId, "DirectionalLightComponent", Json.encode({
        gameObjectName = "MainLightComp",
        rotation = {x = 0, y = 0, z = 0},
        direction = {x = -0.5, y = -0.5, z = 0},
        ambient = {r = 0.02, g = 0.02, b = 0.02},
        diffuse = {r = 1.68, g = 1.5, b = 1.5},
        specular = {r = 0.4, g = 0.4, b = 0.4},
        is_enabled = true,
        is_visible = true,
        shadowAtlasSize = 512
    }))

    -- local a_pointLightId = _CreateActor(host, "Actor",
    -- 	"PointlLightActor",
    -- 	10, 10, 10,
    -- 	0, 0, 0,
    -- 	1, 1, 1,
    -- 	"")

    -- 	_CreateAndAttachComponentToActor(host, a_pointLightId, "PointLightComponent",
    -- 	Json.encode(
    -- 		{
    -- 			gameObjectName = "PointLightComp",
    -- 			translation = {x = 0, y = 0, z = 0},
    -- 			ambient = { r = 0.2, g = 0.2, b = 0.2 },
    -- 			diffuse = { r = 0.68, g = 0.5, b = 0.2 },
    -- 			specular = { r = 0.4, g = 0.4, b = 0.4 },
    -- 			attenuation = {x = 1, y = 1, z = 1},
    --          radianceRadius = 100.0,
    -- 			is_enabled = true,
    -- 			is_visible = true,
    -- 			shadowAtlasSize = 256
    -- 		}
    -- 	))

    local a_spotLightId = _CreateActor(host, "Actor", "SpotPointlLightActor", 10, 10, 10, 0, 0, 0, 1, 1, 1, "")

    _CreateAndAttachComponentToActor(host, a_spotLightId, "SpotlightComponent", Json.encode({
        gameObjectName = "SpotlightComp",
        translation = {x = 0, y = 5, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        ambient = {r = 0.0, g = 0.0, b = 1.0},
        diffuse = {r = 1.0, g = 0.0, b = 0.0},
        specular = {r = 0.4, g = 0.0, b = 0.0},
        attenuation = {x = 1, y = 1, z = 1},
        radianceRadius = 100.0,
        cutoff = 0.75,
        is_enabled = true,
        is_visible = true,
        shadowAtlasSize = 256
    }))

    -- **************************** GROUND ***************************** --
    local a_ground = _CreateActor(host, "Actor", "Ground", 0, 0, 0, 0, 0, 0, 1, 1, 1, "")

    local groundMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, groundMat, "brick_mid.jpg", "albedo")
    _SetTextureToMaterial(host, groundMat, "brick_nm_mid.jpg", "normalMap")
    _SetTextureToMaterial(host, groundMat, "dummy_metallic_roughness.png", "roughnessMap")
    _SetTextureToMaterial(host, groundMat, "dummy_metallic_roughness.png", "metallicMap")
    _SetFloatToMaterial(host, groundMat, 5.0, "uvScale")

    _CreateAndAttachComponentToActor(host, a_ground, "StaticMeshComponent_Deferred", Json.encode({
        gameObjectName = "GroundMeshComponent",
        meshName = "cube.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 50, y = 1, z = 50},
        materialProxyId = groundMat
    }))

    _CreateAndAttachComponentToActor(host, a_ground, "RigidBodyPhysicsComponent", Json.encode({
        gameObjectName = "GroundPhysicsComponent",
        collisionShape = "box",
        halfExtent = {x = 50, y = 1, z = 50},
        physicsBodyType = PhysicsBodyType.STATIC,
        mass = 0.0
    }))

    -- **************************** Platform ***************************** --
    local a_platform = _CreateActor(host, "Actor", "Platform", 0, 10, 0, 0, 0, 0, 1, 1, 1, "")

    local platformMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, platformMat, "brick_mid.jpg", "albedo")
    _SetTextureToMaterial(host, platformMat, "brick_nm_mid.jpg", "normalMap")
    _SetTextureToMaterial(host, platformMat, "dummy_metallic_roughness.png", "roughnessMap")
    _SetTextureToMaterial(host, platformMat, "dummy_metallic_roughness.png", "metallicMap")
    _SetFloatToMaterial(host, platformMat, 1.0, "uvScale")

    _CreateAndAttachComponentToActor(host, a_platform, "StaticMeshComponent_Deferred", Json.encode({
        gameObjectName = "PlatformdMeshComponent",
        meshName = "cube.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 8, y = 1, z = 8},
        materialProxyId = platformMat
    }))

    _CreateAndAttachComponentToActor(host, a_platform, "PlatformTraverseComponent", Json.encode({
        gameObjectName = "PlatformTraverseComponent",
        scriptName = "platformMovementComponentAction.lua",
        routePoints = {
            a1 = {
                translation = {x = 0, y = 0, z = 0},
                rotation = {x = 0, y = 90, z = 0},
                scale = {x = 1, y = 1, z = 1},
                transitionTime = 1.8
            },
            a2 = {
                translation = {x = 10, y = 0, z = 0},
                rotation = {x = 0, y = 90, z = 0},
                scale = {x = 1, y = 1, z = 1},
                transitionTime = 1.8
            },
            a3 = {
                translation = {x = 0, y = 0, z = 10},
                rotation = {x = 0, y = 90, z = 0},
                scale = {x = 1, y = 1, z = 1},
                transitionTime = 1.8
            },
            a4 = {
                translation = {x = -10, y = 0, z = -10},
                rotation = {x = 0, y = 90, z = 0},
                scale = {x = 1, y = 1, z = 1},
                transitionTime = 1.8
            }
        }
    }))

    _CreateAndAttachComponentToActor(host, a_platform, "RigidBodyPhysicsComponent", Json.encode({
        gameObjectName = "PlatformPhysicsComponent",
        collisionShape = "box",
        halfExtent = {x = 8, y = 1, z = 8},
        physicsBodyType = PhysicsBodyType.STATIC,
        mass = 0.0
    }))

    -- **************************** Wall ***************************** --
    local a_wall = _CreateActor(host, "Actor", "WallActor", 35, 8, 0, 0, 0, 90, 1, 1, 1, "")

    local wallMaterial = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_albedo.jpg", "albedo")
    _SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_normal.jpg", "normalMap")
    _SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_roughness.jpg", "roughnessMap")
    _SetTextureToMaterial(host, wallMaterial, "Brick_Medieval_metallic.jpg", "metallicMap")
    _SetFloatToMaterial(host, wallMaterial, 1, "uvScale")

    _CreateAndAttachComponentToActor(host, a_wall, "StaticMeshComponent_Deferred", Json.encode({
        gameObjectName = "WallMeshComponent",
        meshName = "cube.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 8, y = 1, z = 8},
        materialProxyId = wallMaterial
    }))

    _CreateAndAttachComponentToActor(host, a_wall, "RigidBodyPhysicsComponent", Json.encode({
        gameObjectName = "WallPhysicsComponent",
        collisionShape = "box",
        halfExtent = {x = 8, y = 1, z = 8},
        physicsBodyType = PhysicsBodyType.STATIC,
        mass = 0.0
    }))

    -- ***************************HOUSE******************** --
    local a_house = _CreateActor(host, "Actor", "House", 0, 10, 0, 0, 0, 0, 1, 1, 1, "")

    local houseMaterial = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, houseMaterial, "city_house_2_Col.jpg", "albedo")
    _SetTextureToMaterial(host, houseMaterial, "city_house_2_Nor.jpg", "normalMap")
    _SetTextureToMaterial(host, houseMaterial, "city_house_2_Spec.png", "roughnessMap")
    _SetTextureToMaterial(host, houseMaterial, "dummy_metallic_roughness.png", "metallicMap")
    _SetFloatToMaterial(host, houseMaterial, 1, "uvScale")

    _CreateAndAttachComponentToActor(host, a_house, "StaticMeshComponent_Deferred", Json.encode({
        gameObjectName = "HosueMeshComponent",
        meshName = "italian_house_1.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 15, y = 15, z = 15},
        materialProxyId = houseMaterial
    }))

    _CreateAndAttachComponentToActor(host, a_house, "RigidBodyPhysicsComponent", Json.encode({
        gameObjectName = "HousePhysicsComponent",
        collisionShape = "box",
        halfExtent = {x = 7.5, y = 7.5, z = 7.5},
        physicsBodyType = PhysicsBodyType.DYNAMIC,
        mass = 500.0
    }))

    local a_grave = _CreateActor(host, "Actor", "Grave", 15, 5, 0, 0, 0, 0, 1, 1, 1, "")

    local graveMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, graveMat, "Brick_Medieval_albedo.jpg", "albedo")
    _SetTextureToMaterial(host, graveMat, "Brick_Medieval_normal.jpg", "normalMap")
    _SetTextureToMaterial(host, graveMat, "Brick_Medieval_roughness.jpg", "roughnessMap")
    _SetTextureToMaterial(host, graveMat, "Brick_Medieval_metallic.jpg", "metallicMap")
    _SetFloatToMaterial(host, graveMat, 5.0, "uvScale")

    _CreateAndAttachComponentToActor(host, a_grave, "StaticMeshComponent_Deferred", Json.encode({
        gameObjectName = "GraveMeshComponent",
        meshName = "witcher.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 1.5, y = 1.5, z = 1.5},
        materialProxyId = graveMat
    }))

    _CreateAndAttachComponentToActor(host, a_grave, "RigidBodyPhysicsComponent", Json.encode({
        gameObjectName = "HousePhysicsComponent",
        physicsBodyType = PhysicsBodyType.DYNAMIC,
        mass = 1000.0,
        collisionShape = "compoundShape",
        subshapes = {
            leftSphere = {
                collisionShape = "sphere",
                radius = 5,
                translation = {x = -4, y = 0, z = 0},
                rotation = {x = 0, y = 0, z = 0}
            },
            rightSphere = {
                collisionShape = "sphere",
                radius = 5,
                translation = {x = 4, y = 0, z = 0},
                rotation = {x = 0, y = 0, z = 0}
            }
        }
    }))

    -- ***************************SKELET******************** --

    local a_skelet = _CreateActor(host, "Actor", "SkeletActor", 10, 50, 10, 0, 0, 0, 1, 1, 1, "")

    local skeletMat = _CreateMaterial(host, "PhysicalBasedMaterial.m")
    _SetTextureToMaterial(host, skeletMat, "dummy_nm.png", "albedo")
    _SetTextureToMaterial(host, skeletMat, "dummy_nm.png", "normalMap")
    _SetTextureToMaterial(host, skeletMat, "dummy_metallic_roughness.png", "roughnessMap")
    _SetTextureToMaterial(host, skeletMat, "dummy_metallic_roughness.png", "metallicMap")
    _SetFloatToMaterial(host, skeletMat, 1.0, "uvScale")

    _CreateAndAttachComponentToActor(host, a_skelet, "CharacterPhysicsComponent", Json.encode({
        gameObjectName = "HousePhysicsComponent",
        capsuleRadius = 2.5,
        capsuleHeight = 10,
        stepHeight = 1.0,
        mass = 500
    }))

    _CreateAndAttachComponentToActor(host, a_skelet, "HumanoidPhysicsMovementComponent", Json.encode({
        gameObjectName = "SkeletMovementComponent",
        launchDirection = {x = 0, y = 0, z = 0},
        cameraName = "MainCamera"
    }))

    _CreateAndAttachComponentToActor(host, a_skelet, "SkeletalMeshComponent", Json.encode({
        gameObjectName = "SkeletMeshComponent",
        meshName = "tina.fbx",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 90},
        scale = {x = 8, y = 8, z = 8},
        materialProxyId = skeletMat
    }))

    _CreateAndAttachComponentToActor(host, a_skelet, "InputComponent",
                                     Json.encode({gameObjectName = "SkeletInputComponent"}))

    -- _CreateActorController(host, "DefaultActorControllerCreatorFactory",
    --                        "SkeletActor", "HumanoidPlayerController",
    --                        Json.encode({cameraName = "MainCamera"}))

    -- local skeletAnimationTweener = _CreateTweener(host, a_skelet, "playerAnimation.tween")
    -- _SetTweenerBinding(host, a_skelet, skeletAnimationTweener, "SkeletMeshComponent", "animationBinding", "")

    -- -- ****************************WATER***************************** --
    local a_water = _CreateActor(host, "Actor", "WaterActor", 20, 2, 0, 0, 0, 0, 1, 1, 1, "")

    local waterMat = _CreateMaterial(host, "WaterMaterial.m")
    _SetDeferredTextureToMaterial(host, waterMat, "PlanarReflectionComponent", "reflectionTexture")
    _SetTextureToMaterial(host, waterMat, "water_dudv.jpg", "dudv")
    _SetTextureToMaterial(host, waterMat, "brick_mid.jpg", "ground")
    _SetBindingToMaterial(host, waterMat, "EngineScene", "GT_DeltaSec", "deltaTimeSec")
    _SetFloatToMaterial(host, waterMat, 0.5, "mul_coef")

    _CreateAndAttachComponentToActor(host, a_water, "StaticMeshComponent_Forward", Json.encode({
        gameObjectName = "WaterMeshComponent",
        meshName = "plane.obj",
        translation = {x = 0, y = 0, z = 0},
        rotation = {x = 0, y = 0, z = 0},
        scale = {x = 20, y = 1, z = 20},
        materialProxyId = waterMat
    }))

    -- **************************** Particles ***************************** --

    local particlesMat = _CreateMaterial(host, "OpaqueParticleMaterial.m")
    _SetFloatToMaterial(host, particlesMat, 1.0, "opacity")
    _SetFloatToMaterial(host, particlesMat, 0.15, "clipRadius")

    local a_particle = _CreateActor(host, "Actor", "ParticlesActor", 0, 30, 0, 0, 0, 0, 1, 1, 1, "")
    _CreateAndAttachComponentToActor(host, a_particle, "GpuParticleSystemComponent", Json.encode({
        gameObjectName = "c_particleSystem",
        translation = {x = 0.0, y = 0.0, z = 0.0},
        scale = {x = 1.0, y = 1.0, z = 1.0},
        particlesCount = 2000,
        materialProxyId = particlesMat,

        emitter = {type = "explosion", radius = 5.0, thetaSlicesCount = 100},
        lifetime = {type = "simple", lifeTime = 2.0},
        color = {type = "simple", colorBegin = {r = 1.0, g = 0.7, b = 0.2}, colorEnd = {r = 1.0, g = 0.2, b = 0.02}},
        size = {type = "simple", sizeBegin = 0.9, sizeEnd = 0.1},
        velocityModules = {
            {
                type = "simple",
                velocityDirection = {x = 0.0, y = -1.0, z = 0.0},
                velocityDeviation = {x = 0.0, y = 0.0, z = 0.0},
                speed = 15.0
            }
        }
    }))

    local a_particle_2 = _CreateActor(host, "Actor", "ParticlesActor_2", 0, 50, 0, 0, 0, 0, 1, 1, 1, "")
    _CreateAndAttachComponentToActor(host, a_particle_2, "GpuParticleSystemComponent", Json.encode({
        gameObjectName = "c_particleSystem_2",
        translation = {x = 0.0, y = 0.0, z = 0.0},
        scale = {x = 1.0, y = 1.0, z = 1.0},
        particlesCount = 2000,
        materialProxyId = particlesMat,

        emitter = {type = "explosion", radius = 5.0, thetaSlicesCount = 100},
        lifetime = {type = "simple", lifeTime = 2.0},
        color = {type = "simple", colorBegin = {r = 0.0, g = 0.7, b = 1.0}, colorEnd = {r = 0.0, g = 0.02, b = 0.8}},
        size = {type = "simple", sizeBegin = 0.9, sizeEnd = 0.1},
        velocityModules = {
            {
                type = "simple",
                velocityDirection = {x = 0.0, y = -1.0, z = 0.0},
                velocityDeviation = {x = 0.0, y = 0.0, z = 0.0},
                speed = 10.0
            }
        }
    }))

    local a_particle_3 = _CreateActor(host, "Actor", "ParticlesActor_3", 20, 30, 0, 0, 0, 0, 1, 1, 1, "")
    _CreateAndAttachComponentToActor(host, a_particle_3, "GpuParticleSystemComponent", Json.encode({
        gameObjectName = "c_particleSystem_3",
        translation = {x = 0.0, y = 0.0, z = 0.0},
        scale = {x = 1.0, y = 1.0, z = 1.0},
        particlesCount = 2000,
        materialProxyId = particlesMat,

        emitter = {type = "explosion", radius = 5.0, thetaSlicesCount = 100},
        lifetime = {type = "simple", lifeTime = 2.0},
        color = {type = "simple", colorBegin = {r = 0.0, g = 1.0, b = 0.7}, colorEnd = {r = 0.0, g = 0.8, b = 0.02}},
        size = {type = "simple", sizeBegin = 0.9, sizeEnd = 0.1},
        velocityModules = {
            {
                type = "simple",
                velocityDirection = {x = 0.0, y = -1.0, z = 0.0},
                velocityDeviation = {x = 0.0, y = 0.0, z = 0.0},
                speed = 10.0
            }
        }
    }))

    -- **************************** Billboard ***************************** --
    local billboardMat = _CreateMaterial(host, "BillboardMaterial.m")
    _SetTextureToMaterial(host, billboardMat, "perlin_noise_128x128.png", "albedo")

    local a_billboard = _CreateActor(host, "Actor", "BillboardActor", 0, 40, 0, 0, 0, 0, 1, 1, 1, "")
    _CreateAndAttachComponentToActor(host, a_billboard, "BillboardComponent", Json.encode({
        gameObjectName = "BillboardComp",
        translation = {x = 0, y = 0, z = 0},
        rotationRadians = math.rad(180),
        isFlipped = true,
        scale = {x = 1, y = 1, z = 1},
        billboardExtent = 1.0,
        enableScreenAspectRatio = true,
        is_enabled = true,
        is_visible = true,
        materialProxyId = billboardMat
    }))

    _CreateAndAttachComponentToActor(host, a_billboard, "LuaScriptComponent", Json.encode(
                                         {
            gameObjectName = "TestLevelBehaviorScript",
            scriptName = "testLevelBehaviorScript.lua"
        }))
end

function System_OnStart(host) CreateTestLevel(host) end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
