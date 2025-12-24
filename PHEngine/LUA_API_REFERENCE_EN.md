# PHEngine Lua API Reference

## Table of Contents
1. [Introduction](#introduction)
2. [Common Engine Functions](#common-engine-functions)
3. [UI Functions](#ui-functions)
4. [Object Creation Functions](#object-creation-functions)
5. [Event Functions](#event-functions)
6. [Game Functions (GalaxyDefense)](#game-functions-galaxydefense)
7. [Lua Proxy Objects](#lua-proxy-objects)

---

## Introduction

PHEngine provides an extensive Lua API for creating game logic, managing UI, creating scene objects, and handling events. All C++ functions are registered in Lua through a binding system and are accessible from Lua scripts.

Main API categories:
- **LuaCommonEngineFunctions** - basic engine functions (window, settings, input)
- **LuaCommonUiFunctions** - user interface management
- **LuaEngineObjectsCreatorFunctions** - creating actors, components, cameras, materials
- **LuaEngineEventsFunctions** - system event handling
- **LuaGameEventsFunctions** - game events (GalaxyDefense-specific)

---

## Common Engine Functions

### `_GetWindowHeight()`
Returns the window height in pixels.

**Returns:** `integer` - window height

**Example:**
```lua
local height = _GetWindowHeight()
print("Window height: " .. height)
```

---

### `_GetWindowWidth()`
Returns the window width in pixels.

**Returns:** `integer` - window width

**Example:**
```lua
local width = _GetWindowWidth()
print("Window width: " .. width)
```

---

### `_GetMusicGain()`
Returns the current music volume.

**Returns:** `float` - music volume value (0.0 - 1.0)

**Example:**
```lua
local musicVolume = _GetMusicGain()
```

---

### `_GetSoundGain()`
Returns the current sound effects volume.

**Returns:** `float` - sound volume value (0.0 - 1.0)

**Example:**
```lua
local soundVolume = _GetSoundGain()
```

---

### `_HasPressedKeyboardButtons()`
Checks if any keyboard keys were pressed.

**Returns:** `boolean` - true if there are pressed keys

**Example:**
```lua
if _HasPressedKeyboardButtons() then
    local keyData = _GetKeyboardJsonData()
    -- process key presses
end
```

---

### `_HasReleasedKeyboardButtons()`
Checks if any keyboard keys were released.

**Returns:** `boolean` - true if there are released keys

---

### `_GetKeyboardJsonData()`
Returns keyboard state data in JSON format.

**Returns:** `string` - JSON string with key data

**Example:**
```lua
local keyboardData = _GetKeyboardJsonData()
local data = json.decode(keyboardData)
```

---

## UI Functions

### `_GetCurrentOverlayName()`
Returns the name of the currently open overlay (UI screen).

**Returns:** `string` - overlay name

**Example:**
```lua
local currentOverlay = _GetCurrentOverlayName()
print("Current overlay: " .. currentOverlay)
```

---

### `_OpenOverlay(overlayName)`
Opens the specified overlay.

**Parameters:**
- `overlayName` (string) - name of the overlay to open

**Example:**
```lua
_OpenOverlay("MainMenu")
```

---

### `_OpenBackgroundOverlay(overlayName)`
Opens an overlay in background mode (doesn't close the current one).

**Parameters:**
- `overlayName` (string) - name of the background overlay

**Example:**
```lua
_OpenBackgroundOverlay("LoadingScreen")
```

---

### `_CloseCurrentOverlay()`
Closes the current overlay.

**Example:**
```lua
_CloseCurrentOverlay()
```

---

### `_CloseOverlayAndClearHistory()`
Closes the current overlay and clears the overlay history.

**Example:**
```lua
_CloseOverlayAndClearHistory()
```

---

### `_CloseBackgroundOverlay(overlayName)`
Closes the specified background overlay.

**Parameters:**
- `overlayName` (string) - name of the background overlay

---

### `_CreateCommonUiWidget(widgetType, jsonParameters)`
Creates a UI widget of the specified type.

**Parameters:**
- `widgetType` (integer) - widget type (enum)
- `jsonParameters` (string) - JSON string with widget parameters

**Returns:** `integer` - Lua proxy ID of the created widget

**Example:**
```lua
local widgetId = _CreateCommonUiWidget(
    UIWidgetType.Label,
    '{"text": "Hello", "x": 100, "y": 100}'
)
```

---

### `_DestroyCommonUiWidget(luaProxyId)`
Destroys a UI widget by its ID.

**Parameters:**
- `luaProxyId` (integer) - widget's Lua proxy ID

**Example:**
```lua
_DestroyCommonUiWidget(widgetId)
```

---

### `_IsLuaProxyReady(luaProxyId)`
Checks if a Lua proxy object is ready.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID

**Returns:** `boolean` - true if proxy is ready

**Example:**
```lua
if _IsLuaProxyReady(widgetId) then
    -- work with widget
end
```

---

### `_GetUiWidgetName(luaProxyId)`
Returns the UI widget's name.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID

**Returns:** `string` - widget name

---

### `_OnCommonUiWidgetDataUpdated(luaProxyId, jsonData)`
Updates widget data from Lua.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID
- `jsonData` (string) - JSON string with updated data

**Example:**
```lua
_OnCommonUiWidgetDataUpdated(widgetId, '{"visible": true, "opacity": 0.8}')
```

---

### `_GetGameThreadData(luaProxyId)`
Gets widget data from the game thread.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID

**Returns:** `string` - JSON string with data

---

### `_GetMouseInputData(luaProxyId)`
Gets mouse input data for the widget.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID

**Returns:** `string` - JSON string with mouse data

---

### `_InitializeCanvasInputSystem(luaProxyId)`
Initializes the input system for a canvas.

**Parameters:**
- `luaProxyId` (integer) - canvas Lua proxy ID

---

### `_EnableMouseInputReceiverBase(luaProxyId)`
Enables the base mouse input receiver for the widget.

**Parameters:**
- `luaProxyId` (integer) - Lua proxy ID

---

### `_EnableToggleButtonMouseInputReceiver(luaProxyId)`
Enables the mouse input receiver for a toggle button.

**Parameters:**
- `luaProxyId` (integer) - button's Lua proxy ID

---

### `_EnableSliderMouseInputReceiver(luaProxyId)`
Enables the mouse input receiver for a slider.

**Parameters:**
- `luaProxyId` (integer) - slider's Lua proxy ID

---

### `_SetUiWidgetParent(luaProxyId, canvasName, parentName)`
Sets the parent for a UI widget.

**Parameters:**
- `luaProxyId` (integer) - widget's Lua proxy ID
- `canvasName` (string) - canvas name
- `parentName` (string) - parent widget name

---

### `_AddCanvasFadeAnimation(luaProxyId, animationType, animationJsonData)`
Adds a fade in/out animation to a canvas.

**Parameters:**
- `luaProxyId` (integer) - canvas Lua proxy ID
- `animationType` (integer) - 0 for fade-in, 1 for fade-out
- `animationJsonData` (string) - JSON string with animation parameters

**Example:**
```lua
_AddCanvasFadeAnimation(canvasId, 0, '{"duration": 1.0, "delay": 0.0}')
```

---

### `_AddUiItemAnimation(luaProxyId, animationName, animationJsonData)`
Adds an animation to a UI element.

**Parameters:**
- `luaProxyId` (integer) - element's Lua proxy ID
- `animationName` (string) - animation name
- `animationJsonData` (string) - JSON string with animation parameters

**Example:**
```lua
_AddUiItemAnimation(
    widgetId,
    "slide_in",
    '{"property": "position", "from": [0, -100], "to": [0, 0], "duration": 0.5}'
)
```

---

### `_AddUiItemSequenceAnimation(luaProxyId, animationName, animationJsonData)`
Adds a sequence animation to a UI element.

**Parameters:**
- `luaProxyId` (integer) - element's Lua proxy ID
- `animationName` (string) - sequence name
- `animationJsonData` (string) - JSON string with animation sequence

---

### `_StartUiItemAnimation(luaProxyId, animationName)`
Starts a UI element animation.

**Parameters:**
- `luaProxyId` (integer) - element's Lua proxy ID
- `animationName` (string) - animation name to start

**Example:**
```lua
_StartUiItemAnimation(widgetId, "slide_in")
```

---

### `_StartUiItemSequenceAnimation(luaProxyId, animationName)`
Starts a sequence animation for a UI element.

**Parameters:**
- `luaProxyId` (integer) - element's Lua proxy ID
- `animationName` (string) - sequence name

---

## Object Creation Functions

### `_LazyLoadResourcesAsync(resourceNames)`
Asynchronously loads resources by name.

**Parameters:**
- `resourceNames` (string) - resource names separated by commas

**Example:**
```lua
_LazyLoadResourcesAsync("texture1, model2, sound3")
```

---

### `_OpenAudioStreams(audioNames)`
Opens audio streams for streaming playback.

**Parameters:**
- `audioNames` (string) - audio file names separated by commas

**Example:**
```lua
_OpenAudioStreams("music_bg, ambient_sound")
```

---

### `_CreateActor(actorType, actorName, translation, rotation, scale, jsonParams)`
Creates an actor (game object) in the scene.

**Parameters:**
- `actorType` (string) - actor type
- `actorName` (string) - actor name
- `translation` (vec3) - root component position
- `rotation` (vec3) - rotation in degrees
- `scale` (vec3) - scale
- `jsonParams` (string) - additional parameters in JSON

**Returns:** `integer` - ID of the created actor

**Example:**
```lua
local actorId = _CreateActor(
    "Actor",
    "MyActor",
    {0, 0, 0},    -- position
    {0, 0, 0},    -- rotation
    {1, 1, 1},    -- scale
    '{}'          -- additional params
)
```

---

### `_CreateAndAttachComponentToActor(actorId, componentType, componentDataJson)`
Creates a component and attaches it to an actor.

**Parameters:**
- `actorId` (integer) - actor ID
- `componentType` (string) - component type (see supported types below)
- `componentDataJson` (string) - component parameters in JSON

**Supported Component Types:**

#### StaticMeshComponent_Deferred / StaticMeshComponent_Forward / SkeletalMeshComponent
```lua
_CreateAndAttachComponentToActor(actorId, "StaticMeshComponent_Deferred", Json.encode({
    gameObjectName = "MeshComp",
    meshName = "cube.obj",
    translation = {x = 0, y = 0, z = 0},
    rotation = {x = 0, y = 0, z = 0},
    scale = {x = 1, y = 1, z = 1},
    luaScriptName = "",
    materialProxyId = materialId
}))
```

#### RigidBodyPhysicsComponent
```lua
_CreateAndAttachComponentToActor(actorId, "RigidBodyPhysicsComponent", Json.encode({
    gameObjectName = "PhysicsComp",
    collisionShape = "box",  -- or "sphere", "capsule", "plane", "compoundShape"
    halfExtent = {x = 1, y = 1, z = 1},  -- for box
    -- radius = 1.0,  -- for sphere/capsule
    -- height = 2.0,  -- for capsule
    physicsBodyType = 0,  -- 0=STATIC, 1=DYNAMIC, 2=KINEMATIC
    mass = 1.0
}))
```

#### CharacterPhysicsComponent
```lua
_CreateAndAttachComponentToActor(actorId, "CharacterPhysicsComponent", Json.encode({
    gameObjectName = "CharacterPhysics",
    capsuleRadius = 0.5,
    capsuleHeight = 1.8,
    stepHeight = 0.3,
    mass = 80.0
}))
```

#### DirectionalLightComponent
```lua
_CreateAndAttachComponentToActor(actorId, "DirectionalLightComponent", Json.encode({
    gameObjectName = "DirLight",
    rotation = {x = 0, y = 0, z = 0},
    direction = {x = -1, y = -1, z = 0},
    ambient = {r = 0.1, g = 0.1, b = 0.1},
    diffuse = {r = 1.0, g = 1.0, b = 1.0},
    specular = {r = 0.5, g = 0.5, b = 0.5},
    is_enabled = true,
    is_visible = true,
    shadowAtlasSize = 1024  -- optional
}))
```

#### PointLightComponent
```lua
_CreateAndAttachComponentToActor(actorId, "PointLightComponent", Json.encode({
    gameObjectName = "PointLight",
    translation = {x = 0, y = 5, z = 0},
    ambient = {r = 0.1, g = 0.1, b = 0.1},
    diffuse = {r = 1.0, g = 0.5, b = 0.2},
    specular = {r = 0.5, g = 0.5, b = 0.5},
    attenuation = {x = 1, y = 0.09, z = 0.032},
    radianceRadius = 50.0,
    is_enabled = true,
    is_visible = true,
    shadowAtlasSize = 512  -- optional
}))
```

#### SpotlightComponent
```lua
_CreateAndAttachComponentToActor(actorId, "SpotlightComponent", Json.encode({
    gameObjectName = "Spotlight",
    translation = {x = 0, y = 10, z = 0},
    rotation = {x = -90, y = 0, z = 0},
    ambient = {r = 0.0, g = 0.0, b = 0.0},
    diffuse = {r = 1.0, g = 1.0, b = 1.0},
    specular = {r = 1.0, g = 1.0, b = 1.0},
    attenuation = {x = 1, y = 0.09, z = 0.032},
    radianceRadius = 100.0,
    cutoff = 0.85,  -- cosine of cutoff angle
    is_enabled = true,
    is_visible = true,
    shadowAtlasSize = 512  -- optional
}))
```

#### SkyboxComponent
```lua
_CreateAndAttachComponentToActor(actorId, "SkyboxComponent", Json.encode({
    gameObjectName = "Skybox",
    scale = {x = 100, y = 100, z = 100},
    materialProxyId = skyboxMaterialId
}))
```

#### HumanoidPhysicsMovementComponent
```lua
_CreateAndAttachComponentToActor(actorId, "HumanoidPhysicsMovementComponent", Json.encode({
    gameObjectName = "Movement",
    launchDirection = {x = 0, y = 0, z = 0},
    cameraName = "MainCamera"
}))
```

#### PlatformTraverseComponent
```lua
_CreateAndAttachComponentToActor(actorId, "PlatformTraverseComponent", Json.encode({
    gameObjectName = "PlatformTraverse",
    scriptName = "platformMovement.lua",
    routePoints = {
        point1 = {
            translation = {x = 0, y = 0, z = 0},
            rotation = {x = 0, y = 0, z = 0},
            scale = {x = 1, y = 1, z = 1},
            transitionTime = 2.0
        },
        point2 = {
            translation = {x = 10, y = 0, z = 0},
            rotation = {x = 0, y = 0, z = 0},
            scale = {x = 1, y = 1, z = 1},
            transitionTime = 2.0
        }
    }
}))
```

#### BillboardComponent
```lua
_CreateAndAttachComponentToActor(actorId, "BillboardComponent", Json.encode({
    gameObjectName = "Billboard",
    billboardExtent = 1.0,
    enableScreenAspectRatio = true,
    rotationRadians = 0.0,
    isFlipped = false,
    translation = {x = 0, y = 5, z = 0},
    scale = {x = 1, y = 1, z = 1},
    materialProxyId = billboardMaterialId
}))
```

#### InputComponent / UiInputComponent
```lua
_CreateAndAttachComponentToActor(actorId, "InputComponent", Json.encode({
    gameObjectName = "Input"
}))
```

#### GhostPhysicsComponent
```lua
_CreateAndAttachComponentToActor(actorId, "GhostPhysicsComponent", Json.encode({
    gameObjectName = "GhostPhysics",
    collisionShape = "sphere",
    radius = 1.0,
    mass = 0.0
}))
```

#### ElectricBeamComponent
Component for creating electric beam effects between two points.

**Render Modes:**
- `Lines` (0) - Uses RuntimeGeneratedLineComponent (fast, simple)
- `ProceduralMesh` (1) - Uses procedural geometry (realistic, volumetric)
- `ProceduralElectric` (2) - Procedural geometry with jittered segments (most realistic)

```lua
-- Simple line-based mode (default)
_CreateAndAttachComponentToActor(actorId, "ElectricBeamComponent", Json.encode({
    gameObjectName = "ElectricBeam",
    startPoint = {x = 0, y = 0, z = 0},
    endPoint = {x = 10, y = 0, z = 0},
    beamThickness = 2.0,
    beamCount = 3,
    jitterAmount = 0.2,
    updateFrequency = 0.05,
    isActive = true
}))

-- Control render mode programmatically:
-- beam:SetRenderMode(0) -- Lines
-- beam:SetRenderMode(1) -- ProceduralMesh
-- beam:SetRenderMode(2) -- ProceduralElectric (with smooth animation)
-- beam:SetGeometrySegments(8, 10) -- radialSegments, lengthSegments
-- beam:SetAnimationSpeed(2.0) -- Jitter animation speed (0 = static)
```

**Jitter Animation:**
- In `ProceduralElectric` mode, jitter is smoothly animated using noise functions
- Animation happens every frame in the `Tick()` method
- Animation speed controlled via `SetAnimationSpeed()` (default 2.0)
- Each beam in `beamCount` has phase offset for variety

#### ParticleSystemComponent
Particle system component with modular architecture. Supports various emitter types, lifetime, color, size, and velocity modules.

**Module Types:**

**Emitter:**
- `explosion` - emits particles spherically in all directions
  - `radius` - emission sphere radius
  - `thetaSlicesCount` - number of theta angle sectors

**Lifetime:**
- `simple` - simple particle lifetime
  - `lifeTime` - lifetime in seconds

**Color:**
- `simple` - linear interpolation between two colors
  - `colorBegin` - starting color (r, g, b, a)
  - `colorEnd` - ending color (r, g, b, a)

**Size:**
- `simple` - linear interpolation between two sizes
  - `sizeBegin` - starting size
  - `sizeEnd` - ending size

**Velocity Modules:**
- `explosionInitial` - initial explosion velocity (no parameters)
- `simple` - simple velocity with deviation
  - `velocityDirection` - velocity direction (x, y, z)
  - `velocityDeviation` - velocity deviation (x, y, z)
  - `extraVelocityPower` - extra velocity power
- `orbit` - orbital movement
  - `orbitRadius` - orbit radius
  - `orbitHeight` - orbit height
  - `orbitAngularSpeed` - angular rotation speed

**Basic Example:**
```lua
_CreateAndAttachComponentToActor(actorId, "ParticleSystemComponent", Json.encode({
    gameObjectName = "c_particleSystem",
    translation = {x = 0.0, y = 0.0, z = 0.0},
    scale = {x = 1.0, y = 1.0, z = 1.0},
    particlesCount = 100,
    materialProxyId = materialId,
    
    emitter = {
        type = "explosion",
        radius = 1.0,
        thetaSlicesCount = 10
    },
    
    lifetime = {
        type = "simple",
        lifeTime = 2.5
    },
    
    color = {
        type = "simple",
        colorBegin = {r = 1.0, g = 0.7, b = 0.2, a = 1.0},
        colorEnd = {r = 1.0, g = 0.2, b = 0.02, a = 1.0}
    },
    
    size = {
        type = "simple",
        sizeBegin = 0.4,
        sizeEnd = 0.1
    },
    
    velocityModules = {
        {type = "explosionInitial"},
        {
            type = "simple",
            velocityDirection = {x = 0.0, y = -25.0, z = 0.0},
            velocityDeviation = {x = 2.0, y = 0.0, z = 2.0},
            extraVelocityPower = 1.0
        }
    }
}))
```

**Orbital Movement Example:**
```lua
_CreateAndAttachComponentToActor(actorId, "ParticleSystemComponent", Json.encode({
    gameObjectName = "c_particleOrbit",
    translation = {x = 0.0, y = 0.0, z = 0.0},
    scale = {x = 1.0, y = 1.0, z = 1.0},
    particlesCount = 50,
    materialProxyId = materialId,
    
    emitter = {
        type = "explosion",
        radius = 0.5,
        thetaSlicesCount = 8
    },
    
    lifetime = {
        type = "simple",
        lifeTime = 5.0
    },
    
    color = {
        type = "simple",
        colorBegin = {r = 0.2, g = 0.5, b = 1.0, a = 1.0},
        colorEnd = {r = 1.0, g = 0.8, b = 0.2, a = 0.5}
    },
    
    size = {
        type = "simple",
        sizeBegin = 0.3,
        sizeEnd = 0.2
    },
    
    velocityModules = {
        {
            type = "orbit",
            orbitRadius = 3.0,
            orbitHeight = 2.0,
            orbitAngularSpeed = 1.5
        }
    }
}))
```

**Complete Example with All Options:**
```lua
-- Create particle material
local materialParser = MaterialParser()
local particleMaterial = materialParser:ParseMaterialDescriptor("OpaqueParticleMaterial.m")
scene:RegisterMaterialInstance(particleMaterial)

-- Set material properties
MaterialPropertySetter.SetMaterialPropertyValue(particleMaterial, "opacity", 1.0)
MaterialPropertySetter.SetMaterialPropertyValue(particleMaterial, "clipRadius", 0.35)

-- Get material ID
local materialProxyId = particleMaterial:GetMaterialProxyWp():lock():GetSceneProxyId()

-- Create particle component
_CreateAndAttachComponentToActor(actorId, "ParticleSystemComponent", Json.encode({
    gameObjectName = "c_explosion_particles",
    translation = {x = 0.0, y = 5.0, z = 0.0},
    scale = {x = 1.0, y = 1.0, z = 1.0},
    particlesCount = 200,
    materialProxyId = materialProxyId,
    
    emitter = {
        type = "explosion",
        radius = 2.5,
        thetaSlicesCount = 12
    },
    
    lifetime = {
        type = "simple",
        lifeTime = 3.0
    },
    
    color = {
        type = "simple",
        colorBegin = {r = 1.0, g = 1.0, b = 1.0, a = 1.0},
        colorEnd = {r = 0.5, g = 0.0, b = 0.0, a = 0.0}
    },
    
    size = {
        type = "simple",
        sizeBegin = 0.8,
        sizeEnd = 0.05
    },
    
    velocityModules = {
        {type = "explosionInitial"},
        {
            type = "simple",
            velocityDirection = {x = 0.0, y = -10.0, z = 0.0},
            velocityDeviation = {x = 5.0, y = 2.0, z = 5.0},
            extraVelocityPower = 2.0
        }
    }
}))
```

**Important Notes:**
- All modules are optional - if not specified, particles use default values
- Multiple velocity modules can be used simultaneously - they are applied sequentially
- Material must be created and registered before creating the component
- MaterialProxyId is obtained via `material:GetMaterialProxyWp():lock():GetSceneProxyId()`

---

### `_CreatePlanarReflectionComponent(componentDataJson)`
Creates a planar reflection component for water/mirror reflections.

**Parameters:**
- `componentDataJson` (string) - component parameters in JSON

**Example:**
```lua
_CreatePlanarReflectionComponent(Json.encode({
    gameObjectName = "PlanarReflection",
    cameraName = "MainCamera",
    translation = {x = 0, y = 0, z = 0},
    rotation = {x = 0, y = 0, z = 0},
    scale = {x = 1, y = 1, z = 1},
    viewPortX = 0,
    viewPortY = 0,
    viewPortWidth = 1920,
    viewPortHeight = 1080
}))
```

---

### `_CreateThirdPersonCamera(cameraName, viewport, viewProjectionJson, distance, pitch, yaw, offset, isMainCamera)`
Creates a third-person camera.

**Parameters:**
- `cameraName` (string) - camera name
- `viewport` (ivec4) - viewport coordinates {x, y, width, height}
- `viewProjectionJson` (string) - projection parameters in JSON
- `distance` (float) - distance from target
- `pitch` (float) - pitch angle
- `yaw` (float) - yaw angle
- `offset` (vec3) - offset
- `isMainCamera` (integer) - 1 if main camera

**Example:**
```lua
_CreateThirdPersonCamera(
    "MainCamera",
    {0, 0, 1920, 1080},
    '{"fov": 60, "near": 0.1, "far": 1000}',
    5.0,   -- distance
    30.0,  -- pitch
    0.0,   -- yaw
    {0, 2, 0},  -- offset
    1      -- is main
)
```

---

### `_SetCameraThirdPersonTarget(cameraName, targetActorName)`
Sets the target for a third-person camera.

**Parameters:**
- `cameraName` (string) - camera name
- `targetActorName` (string) - target actor name

**Example:**
```lua
_SetCameraThirdPersonTarget("MainCamera", "Player")
```

---

### `_CreateFirstPersonCamera(cameraName, viewport, viewProjectionJson, initPitch, initYaw, initPosition, isMainCamera)`
Creates a first-person camera.

**Parameters:**
- `cameraName` (string) - camera name
- `viewport` (ivec4) - viewport coordinates
- `viewProjectionJson` (string) - projection parameters in JSON
- `initPitch` (float) - initial pitch angle
- `initYaw` (float) - initial yaw angle
- `initPosition` (vec3) - initial position
- `isMainCamera` (integer) - 1 if main camera

**Пример:**
```lua
_CreateFirstPersonCamera(
    "FPSCamera",
    {0, 0, 1920, 1080},
    '{"fov": 90, "near": 0.1, "far": 1000}',
    0.0,   -- pitch
    0.0,   -- yaw
    {0, 1.7, 0},  -- position (eye level)
    1      -- is main
)

---

### `_CreateMaterial(materialName)`
Creates a material instance.

**Parameters:**
- `materialName` (string) - material name

**Returns:** `integer` - material's Lua proxy ID

**Example:**
```lua
local materialId = _CreateMaterial('materialName.m')
```

---

### `_SetTextureToMaterial(materialId, textureName, propertyName)`
Sets a texture to a material.

**Parameters:**
- `materialId` (integer) - material ID
- `textureName` (string) - texture name
- `propertyName` (string) - material property name

**Example:**
```lua
_SetTextureToMaterial(materialId, "brick_diffuse.png", "diffuse")
```

---

### `_SetFloatToMaterial(materialId, value, propertyName)`
Sets a float value to a material.

**Parameters:**
- `materialId` (integer) - material ID
- `value` (float) - value
- `propertyName` (string) - property name

**Example:**
```lua
_SetFloatToMaterial(materialId, 0.8, "metallic")
```

---

### `_SetVec3ToMaterial(materialId, propertyName, valueJson)`
Sets a vec3 value to a material.

**Parameters:**
- `materialId` (integer) - material ID
- `propertyName` (string) - property name
- `valueJson` (string) - JSON with vec3 value

**Example:**
```lua
_SetVec3ToMaterial(materialId, "albedo", '{"x": 1.0, "y": 0.5, "z": 0.2}')
```

---

### `_SetBindingToMaterial(materialId, gameObjectName, gamePropertyName, bindingName)`
Sets a binding to a material property from a game object.

**Parameters:**
- `materialId` (integer) - material ID
- `gameObjectName` (string) - game object name (e.g., "EngineScene")
- `gamePropertyName` (string) - object property name (e.g., "GT_DeltaSec")
- `bindingName` (string) - binding name in material (e.g., "deltaTimeSec")

**Example:**
```lua
-- Bind scene time to material for animated shaders
_SetBindingToMaterial(skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTimeSec")
```

**Use cases:**
This function allows dynamically linking material properties with game objects. For example:
- Passing time for shader animation (water, sky)
- Camera parameter binding
- Linking with light or other scene object parameters

---

### `_CreateTweener(actorId, tweenerJson)`
Creates a tweener (animation) for an actor.

**Parameters:**
- `actorId` (integer) - actor ID
- `tweenerJson` (string) - tweener parameters in JSON

**Returns:** `integer` - tweener ID

**Example:**
```lua
local tweenerId = _CreateTweener(
    actorId,
    '{"duration": 2.0, "loop": true}'
)
```

---

### `_CreateActorController(controllerType, controllerName, actorName, jsonParams)`
Creates a controller for an actor.

**Parameters:**
- `controllerType` (string) - controller type
- `controllerName` (string) - controller name
- `actorName` (string) - actor name
- `jsonParams` (string) - parameters in JSON

**Example:**
```lua
_CreateActorController(
    "PlayerController",
    "Player1Controller",
    "PlayerActor",
    '{"speed": 5.0}'
)
```

---

## Event Functions

### `_SendPauseGameThreadEvent(enqueuePolicy, pauseState)`
Sends a pause/unpause game event.

**Parameters:**
- `enqueuePolicy` (integer) - enqueue policy
- `pauseState` (boolean) - true to pause, false to unpause

**Example:**
```lua
_SendPauseGameThreadEvent(0, true)  -- pause
```

---

## Game Functions (GalaxyDefense)

### `_GetSelectedMissileType()`
Returns the selected missile type.

**Returns:** `integer` - missile type (enum)

**Example:**
```lua
local missileType = _GetSelectedMissileType()
```

---

### `_GetAllMissilesData()`
Returns data about all missiles.

**Returns:** `string` - JSON string with missile data

**Example:**
```lua
local missilesData = _GetAllMissilesData()
local data = json.decode(missilesData)
```

---

### `_GetEnemySpaceshipsCountDestroyedByPlayer()`
Returns the number of enemy spaceships destroyed by the player.

**Returns:** `integer` - number of ships

**Example:**
```lua
local destroyedCount = _GetEnemySpaceshipsCountDestroyedByPlayer()
print("Destroyed: " .. destroyedCount)
```

---

### `_GetEditorLevelAreaBoundingBoxWidth()`
Returns the level area width in the editor.

**Returns:** `float` - width

---

### `_GetEditorLevelAreaBoundingBoxLength()`
Returns the level area length in the editor.

**Returns:** `float` - length

---

### `_SendChangeGameModeGameThreadEvent(enqueuePolicy, gameModeType)`
Sends a game mode change event.

**Parameters:**
- `enqueuePolicy` (integer) - enqueue policy
- `gameModeType` (integer) - game mode type (enum)

**Example:**
```lua
_SendChangeGameModeGameThreadEvent(0, GameMode.Combat)
```

---

### `_SendChangeEditModeGameThreadEvent(enqueuePolicy, editModeType)`
Sends an edit mode change event.

**Parameters:**
- `enqueuePolicy` (integer) - enqueue policy
- `editModeType` (integer) - edit mode type (enum)

---

## Lua Proxy Objects

Lua proxy objects represent C++ objects in Lua code and allow interaction with them.

### UI Proxy Types:
- **UiCanvasLuaProxy** - canvas (UI element container)
- **UiLabelLuaProxy** - text label
- **UiImageLuaProxy** - image
- **UiButtonLuaProxy** - button
- **UiToggleButtonLuaProxy** - toggle button
- **UiSliderLuaProxy** - slider
- **UiProgressBarLuaProxy** - progress bar
- **UiTextBlockLuaProxy** - text block
- **UiRectangleLuaProxy** - rectangle
- **UiGridLayoutLuaProxy** - grid layout
- **UiRowLayoutLuaProxy** - row layout

### Common UI Proxy Methods:

Most UI proxies have the following methods (called with colon syntax):

```lua
-- Set visibility
proxy:SetVisible(true)

-- Set position (normalized coordinates 0-1)
proxy:SetPosition(0.5, 0.5)

-- Set size
proxy:SetSize(200, 100)

-- Set opacity
proxy:SetOpacity(0.8)

-- Set Z-order
proxy:SetZOrder(10)

-- Get name
local name = proxy:GetName()
```

### Specific Methods:

#### UiLabelLuaProxy
```lua
label:SetText("Hello World")
label:SetFontSize(24)
label:SetColor(1.0, 1.0, 1.0)
```

#### UiSliderLuaProxy
```lua
slider:SetValue(0.5)
slider:SetMinValue(0.0)
slider:SetMaxValue(1.0)
slider:SetStep(0.1)
local value = slider:GetValue()
```

#### UiToggleButtonLuaProxy
```lua
button:SetToggleState(true)
local isToggled = button:GetToggleState()
```

### EngineInputLuaProxy

Provides access to the input system:

```lua
local input = -- obtained through LuaScriptProcessor

-- Keyboard
local isPressedKeys = input:GetIsPressedKeyboardKeys()
local isReleasedKeys = input:GetIsReleasedKeyboardKeys()
local keyboardData = input:GetKeyboardJsonData()

-- Mouse
local mouseData = input:GetMouseJsonData()
```

---

## Usage Examples

### Creating a UI Element
```lua
-- Create a label
local labelId = _CreateCommonUiWidget(
    UIWidgetType.Label,
    '{"name": "MyLabel", "x": 0.5, "y": 0.5, "text": "Score: 0"}'
)

-- Wait for proxy to be ready
while not _IsLuaProxyReady(labelId) do
    coroutine.yield()
end

-- Update text
_OnCommonUiWidgetDataUpdated(labelId, '{"text": "Score: 100"}')
```

### Creating an Actor with Components
```lua
-- Load resources
_LazyLoadResourcesAsync("player_model.obj, player_texture.png")

-- Create actor
local playerId = _CreateActor(
    "Actor",
    "Player",
    {0, 0, 0},
    {0, 0, 0},
    {1, 1, 1},
    '{}'
)

-- Add mesh
_CreateAndAttachComponentToActor(
    playerId,
    "MeshComponent",
    '{"mesh": "player_model.obj"}'
)

-- Create material
local matId = _CreateMaterial('{"shader": "PBR"}')
_SetTextureToMaterial(matId, "player_texture.png", "diffuse")
```

### UI Animation
```lua
local buttonId = _CreateCommonUiWidget(UIWidgetType.Button, '{}')

-- Add animation
_AddUiItemAnimation(
    buttonId,
    "pulse",
    [[{
        "property": "scale",
        "from": 1.0,
        "to": 1.2,
        "duration": 0.5,
        "interpolation": "smooth",
        "loop": true
    }]]
)

-- Start animation
_StartUiItemAnimation(buttonId, "pulse")
```

---

## Enums and Constants

### enqueueJobPolicy
- `0` - IF_DUPLICATE_NO_PUSH - don't add duplicates
- `1` - IF_DUPLICATE_REPLACE - replace duplicates
- `2` - PUSH_ANYWAY - always add

### UIWidgetType
- Defined in `eCommonUiWidgetType`
- Canvas, Label, Image, Button, Slider, ProgressBar, etc.

### GameMode (GalaxyDefense)
- Depends on game implementation
- Combat, Edit, Menu, etc.

---

## Notes

1. **Threading**: PHEngine uses multithreading. Lua runs in a separate thread, and some operations (e.g., rendering) run in other threads. The system automatically synchronizes data between threads.

2. **JSON Parameters**: Many functions accept JSON strings to pass complex parameters. Use the `json` library in Lua to work with JSON.

3. **Proxy Readiness**: After creating objects through the API, check proxy readiness with `_IsLuaProxyReady()` before use.

4. **UI Coordinates**: UI uses normalized coordinates (0.0 - 1.0) for positioning, where (0, 0) is the top-left corner, (1, 1) is the bottom-right.

5. **Resources**: Always load resources before use with `_LazyLoadResourcesAsync()`.

---

## Extending the API

To add new functions to the Lua API:

1. Declare the function in the corresponding `LuaXxxFunctions.h` class
2. Implement the function in the `.cpp` file
3. Register it via `LuaCallbackBindingHelper` in the `RegisterCallbacks()` method
4. Use the `_` prefix for the Lua function name

Registration example:
```cpp
LuaCallbackBindingHelper<
    Hash64_CT("MyClass::MyFunction"), 
    ReturnType(Param1Type, Param2Type)
>::Bind(
    luaWrapper,
    mOwnerPtr,
    std::bind(&MyClass::MyFunction, this, std::placeholders::_1),
    "_MyLuaFunctionName"
);
```
