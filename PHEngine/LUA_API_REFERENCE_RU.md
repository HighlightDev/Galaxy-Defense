# PHEngine Lua API Reference

## Оглавление
1. [Введение](#введение)
2. [Общие функции движка](#общие-функции-движка)
3. [Функции UI](#функции-ui)
4. [Функции создания объектов](#функции-создания-объектов)
5. [Функции событий](#функции-событий)
6. [Игровые функции (GalaxyDefense)](#игровые-функции-galaxydefense)
7. [Lua Proxy объекты](#lua-proxy-объекты)

---

## Введение

PHEngine предоставляет обширное Lua API для создания игровой логики, управления UI, создания объектов сцены и обработки событий. Все функции C++ регистрируются в Lua через систему биндингов и доступны из Lua-скриптов.

Основные категории API:
- **LuaCommonEngineFunctions** - базовые функции движка (окно, настройки, ввод)
- **LuaCommonUiFunctions** - работа с пользовательским интерфейсом
- **LuaEngineObjectsCreatorFunctions** - создание акторов, компонентов, камер, материалов
- **LuaEngineEventsFunctions** - обработка системных событий
- **LuaGameEventsFunctions** - игровые события (специфичные для GalaxyDefense)

---

## Общие функции движка

### `_GetWindowHeight()`
Возвращает высоту окна в пикселях.

**Возвращает:** `integer` - высота окна

**Пример:**
```lua
local height = _GetWindowHeight()
print("Window height: " .. height)
```

---

### `_GetWindowWidth()`
Возвращает ширину окна в пикселях.

**Возвращает:** `integer` - ширина окна

**Пример:**
```lua
local width = _GetWindowWidth()
print("Window width: " .. width)
```

---

### `_GetMusicGain()`
Возвращает текущую громкость музыки.

**Возвращает:** `float` - значение громкости музыки (0.0 - 1.0)

**Пример:**
```lua
local musicVolume = _GetMusicGain()
```

---

### `_GetSoundGain()`
Возвращает текущую громкость звуковых эффектов.

**Возвращает:** `float` - значение громкости звуков (0.0 - 1.0)

**Пример:**
```lua
local soundVolume = _GetSoundGain()
```

---

### `_HasPressedKeyboardButtons()`
Проверяет, были ли нажаты клавиши клавиатуры.

**Возвращает:** `boolean` - true если есть нажатые клавиши

**Пример:**
```lua
if _HasPressedKeyboardButtons() then
    local keyData = _GetKeyboardJsonData()
    -- обработка нажатий
end
```

---

### `_HasReleasedKeyboardButtons()`
Проверяет, были ли отпущены клавиши клавиатуры.

**Возвращает:** `boolean` - true если есть отпущенные клавиши

---

### `_GetKeyboardJsonData()`
Возвращает данные о состоянии клавиатуры в формате JSON.

**Возвращает:** `string` - JSON строка с данными о клавишах

**Пример:**
```lua
local keyboardData = _GetKeyboardJsonData()
local data = json.decode(keyboardData)
```

---

## Функции UI

### `_GetCurrentOverlayName()`
Возвращает имя текущего открытого оверлея (UI-экрана).

**Возвращает:** `string` - имя оверлея

**Пример:**
```lua
local currentOverlay = _GetCurrentOverlayName()
print("Current overlay: " .. currentOverlay)
```

---

### `_OpenOverlay(overlayName)`
Открывает указанный оверлей.

**Параметры:**
- `overlayName` (string) - имя оверлея для открытия

**Пример:**
```lua
_OpenOverlay("MainMenu")
```

---

### `_OpenBackgroundOverlay(overlayName)`
Открывает оверлей в фоновом режиме (не закрывает текущий).

**Параметры:**
- `overlayName` (string) - имя фонового оверлея

**Пример:**
```lua
_OpenBackgroundOverlay("LoadingScreen")
```

---

### `_CloseCurrentOverlay()`
Закрывает текущий оверлей.

**Пример:**
```lua
_CloseCurrentOverlay()
```

---

### `_CloseOverlayAndClearHistory()`
Закрывает текущий оверлей и очищает историю оверлеев.

**Пример:**
```lua
_CloseOverlayAndClearHistory()
```

---

### `_CloseBackgroundOverlay(overlayName)`
Закрывает указанный фоновый оверлей.

**Параметры:**
- `overlayName` (string) - имя фонового оверлея

---

### `_CreateCommonUiWidget(widgetType, jsonParameters)`
Создает UI виджет заданного типа.

**Параметры:**
- `widgetType` (integer) - тип виджета (enum)
- `jsonParameters` (string) - JSON строка с параметрами виджета

**Возвращает:** `integer` - ID Lua proxy созданного виджета

**Пример:**
```lua
local widgetId = _CreateCommonUiWidget(
    UIWidgetType.Label,
    '{"text": "Hello", "x": 100, "y": 100}'
)
```

---

### `_DestroyCommonUiWidget(luaProxyId)`
Уничтожает UI виджет по его ID.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy виджета

**Пример:**
```lua
_DestroyCommonUiWidget(widgetId)
```

---

### `_IsLuaProxyReady(luaProxyId)`
Проверяет, готов ли Lua proxy объект.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy

**Возвращает:** `boolean` - true если proxy готов

**Пример:**
```lua
if _IsLuaProxyReady(widgetId) then
    -- работа с виджетом
end
```

---

### `_GetUiWidgetName(luaProxyId)`
Возвращает имя UI виджета.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy

**Возвращает:** `string` - имя виджета

---

### `_OnCommonUiWidgetDataUpdated(luaProxyId, jsonData)`
Обновляет данные виджета из Lua.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy
- `jsonData` (string) - JSON строка с обновленными данными

**Пример:**
```lua
_OnCommonUiWidgetDataUpdated(widgetId, '{"visible": true, "opacity": 0.8}')
```

---

### `_GetGameThreadData(luaProxyId)`
Получает данные виджета из игрового потока.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy

**Возвращает:** `string` - JSON строка с данными

---

### `_GetMouseInputData(luaProxyId)`
Получает данные о вводе мыши для виджета.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy

**Возвращает:** `string` - JSON строка с данными мыши

---

### `_InitializeCanvasInputSystem(luaProxyId)`
Инициализирует систему ввода для канваса.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy канваса

---

### `_EnableMouseInputReceiverBase(luaProxyId)`
Включает базовый приемник ввода мыши для виджета.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy

---

### `_EnableToggleButtonMouseInputReceiver(luaProxyId)`
Включает приемник ввода мыши для toggle button.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy кнопки

---

### `_EnableSliderMouseInputReceiver(luaProxyId)`
Включает приемник ввода мыши для слайдера.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy слайдера

---

### `_SetUiWidgetParent(luaProxyId, canvasName, parentName)`
Устанавливает родителя для UI виджета.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy виджета
- `canvasName` (string) - имя канваса
- `parentName` (string) - имя родительского виджета

---

### `_AddCanvasFadeAnimation(luaProxyId, animationType, animationJsonData)`
Добавляет анимацию затухания/появления канваса.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy канваса
- `animationType` (integer) - 0 для fade-in, 1 для fade-out
- `animationJsonData` (string) - JSON строка с параметрами анимации

**Пример:**
```lua
_AddCanvasFadeAnimation(canvasId, 0, '{"duration": 1.0, "delay": 0.0}')
```

---

### `_AddUiItemAnimation(luaProxyId, animationName, animationJsonData)`
Добавляет анимацию к UI элементу.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy элемента
- `animationName` (string) - имя анимации
- `animationJsonData` (string) - JSON строка с параметрами анимации

**Пример:**
```lua
_AddUiItemAnimation(
    widgetId,
    "slide_in",
    '{"property": "position", "from": [0, -100], "to": [0, 0], "duration": 0.5}'
)
```

---

### `_AddUiItemSequenceAnimation(luaProxyId, animationName, animationJsonData)`
Добавляет последовательную анимацию к UI элементу.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy элемента
- `animationName` (string) - имя последовательности
- `animationJsonData` (string) - JSON строка с последовательностью анимаций

---

### `_StartUiItemAnimation(luaProxyId, animationName)`
Запускает анимацию UI элемента.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy элемента
- `animationName` (string) - имя анимации для запуска

**Пример:**
```lua
_StartUiItemAnimation(widgetId, "slide_in")
```

---

### `_StartUiItemSequenceAnimation(luaProxyId, animationName)`
Запускает последовательную анимацию UI элемента.

**Параметры:**
- `luaProxyId` (integer) - ID Lua proxy элемента
- `animationName` (string) - имя последовательности

---

## Функции создания объектов

### `_LazyLoadResourcesAsync(resourceNames)`
Асинхронно загружает ресурсы по именам.

**Параметры:**
- `resourceNames` (string) - имена ресурсов через запятую

**Пример:**
```lua
_LazyLoadResourcesAsync("texture1, model2, sound3")
```

---

### `_OpenAudioStreams(audioNames)`
Открывает аудио потоки для стриминга.

**Параметры:**
- `audioNames` (string) - имена аудио файлов через запятую

**Пример:**
```lua
_OpenAudioStreams("music_bg, ambient_sound")
```

---

### `_CreateActor(actorType, actorName, translation, rotation, scale, jsonParams)`
Создает актора (игровой объект) в сцене.

**Параметры:**
- `actorType` (string) - тип актора
- `actorName` (string) - имя актора
- `translation` (vec3) - позиция корневого компонента
- `rotation` (vec3) - поворот в градусах
- `scale` (vec3) - масштаб
- `jsonParams` (string) - дополнительные параметры в JSON

**Возвращает:** `integer` - ID созданного актора

**Пример:**
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
Создает компонент и прикрепляет его к актору.

**Параметры:**
- `actorId` (integer) - ID актора
- `componentType` (string) - тип компонента (см. поддерживаемые типы ниже)
- `componentDataJson` (string) - параметры компонента в JSON

**Поддерживаемые типы компонентов:**

#### StaticMeshComponent_Deferred / StaticMeshComponent_Forward / SkeletalMeshComponent
```lua
_CreateAndAttachComponentToActor(actorId, "StaticMeshComponent_Deferred", Json.encode({
    gameObjectName = "MeshComp",
    meshName = "cube.obj",
    translation = {x = 0, y = 0, z = 0},
    rotation = {x = 0, y = 0, z = 0},
    scale = {x = 1, y = 1, z = 1},
    materialProxyId = materialId
}))
```

#### RigidBodyPhysicsComponent
```lua
_CreateAndAttachComponentToActor(actorId, "RigidBodyPhysicsComponent", Json.encode({
    gameObjectName = "PhysicsComp",
    collisionShape = "box",  -- или "sphere", "capsule", "plane", "compoundShape"
    halfExtent = {x = 1, y = 1, z = 1},  -- для box
    -- radius = 1.0,  -- для sphere/capsule
    -- height = 2.0,  -- для capsule
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
    shadowAtlasSize = 1024  -- опционально
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
    shadowAtlasSize = 512  -- опционально
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
    cutoff = 0.85,  -- косинус угла обрезки
    is_enabled = true,
    is_visible = true,
    shadowAtlasSize = 512  -- опционально
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

#### LuaScriptComponent
Компонент, который выполняет Lua-скрипты с функциями `Initialize()` и `Tick(deltaTime)`. Полезен для кастомной игровой логики, привязанной к акторам.

```lua
_CreateAndAttachComponentToActor(actorId, "LuaScriptComponent", Json.encode({
    gameObjectName = "ScriptComp",
    scriptFilePath = "myScript.lua"  -- Путь к Lua-скрипту
}))
```

**Пример файла скрипта (myScript.lua):**
```lua
-- Вызывается один раз при регистрации компонента
function Initialize()
    print("Скрипт инициализирован!")
end

-- Вызывается каждый кадр
function Tick(deltaTime)
    -- deltaTime - время в секундах с прошлого кадра
    -- Добавьте свою игровую логику здесь
end
```

**Примечания:**
- Файл скрипта должен находиться в директории resources/scripts
- Обе функции `Initialize()` и `Tick(deltaTime)` опциональны
- Скрипт имеет доступ ко всем глобальным функциям Lua API
- Используйте этот компонент для реализации пользовательского поведения акторов

---

#### ElectricBeamComponent
Компонент для создания эффектов электрических лучей между двумя точками.

**Режимы рендеринга:**
- `Lines` (0) - Использует RuntimeGeneratedLineComponent (быстро, просто)
- `ProceduralMesh` (1) - Использует процедурную геометрию (реалистично, объемно)
- `ProceduralElectric` (2) - Процедурная геометрия с джиттером сегментов (максимально реалистично)

```lua
-- Простой режим с линиями (по умолчанию)
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

-- Управление режимом рендеринга через код:
-- beam:SetRenderMode(0) -- Lines
-- beam:SetRenderMode(1) -- ProceduralMesh
-- beam:SetRenderMode(2) -- ProceduralElectric (с плавной анимацией)
-- beam:SetGeometrySegments(8, 10) -- radialSegments, lengthSegments
-- beam:SetAnimationSpeed(2.0) -- Скорость анимации джиттера (0 = статично)
```

**Анимация джиттера:**
- В режиме `ProceduralElectric` джиттер анимируется плавно используя шумовую функцию
- Анимация происходит каждый кадр в методе `Tick()`
- Скорость анимации управляется через `SetAnimationSpeed()` (по умолчанию 2.0)
- Каждый луч в `beamCount` имеет смещение по фазе для разнообразия

#### CpuParticleSystemComponent
Компонент системы частиц с модульной архитектурой. Поддерживает различные типы эмиттеров, модули времени жизни, цвета, размера и скорости.

**Типы модулей:**

**Emitter (Эмиттер):**
- `explosion` - испускает частицы сферически во все стороны
  - `radius` - радиус сферы эмиссии
  - `thetaSlicesCount` - количество секторов по theta углу

**Lifetime (Время жизни):**
- `simple` - простое время жизни частиц
  - `lifeTime` - время жизни в секундах

**Color (Цвет):**
- `simple` - линейная интерполяция между двумя цветами
  - `colorBegin` - начальный цвет (r, g, b, a)
  - `colorEnd` - конечный цвет (r, g, b, a)

**Size (Размер):**
- `simple` - линейная интерполяция между двумя размерами
  - `sizeBegin` - начальный размер
  - `sizeEnd` - конечный размер

**Velocity Modules (Модули скорости):**
- `explosionInitial` - начальная скорость при взрыве (без параметров)
- `simple` - простая скорость с отклонением
  - `velocityDirection` - направление скорости (x, y, z)
  - `velocityDeviation` - отклонение скорости (x, y, z)
  - `extraVelocityPower` - дополнительная сила скорости
- `orbit` - орбитальное движение
  - `orbitRadius` - радиус орбиты
  - `orbitHeight` - высота орбиты
  - `orbitAngularSpeed` - угловая скорость вращения

**Базовый пример:**
```lua
_CreateAndAttachComponentToActor(actorId, "CpuParticleSystemComponent", Json.encode({
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

**Пример с орбитальным движением:**
```lua
_CreateAndAttachComponentToActor(actorId, "CpuParticleSystemComponent", Json.encode({
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

**Полный пример со всеми опциями:**
```lua
-- Создание материала для частиц
local materialParser = MaterialParser()
local particleMaterial = materialParser:ParseMaterialDescriptor("OpaqueParticleMaterial.m")
scene:RegisterMaterialInstance(particleMaterial)

-- Настройка свойств материала
MaterialPropertySetter.SetMaterialPropertyValue(particleMaterial, "opacity", 1.0)
MaterialPropertySetter.SetMaterialPropertyValue(particleMaterial, "clipRadius", 0.35)

-- Получение ID материала
local materialProxyId = particleMaterial:GetMaterialProxyWp():lock():GetSceneProxyId()

-- Создание компонента частиц
_CreateAndAttachComponentToActor(actorId, "CpuParticleSystemComponent", Json.encode({
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

**Важные замечания:**
- Все модули опциональны - если не указаны, частицы используют значения по умолчанию
- Можно использовать несколько velocity модулей одновременно - они применяются последовательно
- Material должен быть создан и зарегистрирован до создания компонента
- MaterialProxyId получается через `material:GetMaterialProxyWp():lock():GetSceneProxyId()`

---

### `_CreatePlanarReflectionComponent(componentDataJson)`
Создает компонент планарного отражения для воды/зеркал.

**Параметры:**
- `componentDataJson` (string) - параметры компонента в JSON

**Пример:**
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
Создает камеру от третьего лица.

**Параметры:**
- `cameraName` (string) - имя камеры
- `viewport` (ivec4) - координаты вьюпорта {x, y, width, height}
- `viewProjectionJson` (string) - параметры проекции в JSON
- `distance` (float) - дистанция от цели
- `pitch` (float) - угол наклона
- `yaw` (float) - угол поворота
- `offset` (vec3) - смещение
- `isMainCamera` (integer) - 1 если основная камера

**Пример:**
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
Устанавливает цель для камеры от третьего лица.

**Параметры:**
- `cameraName` (string) - имя камеры
- `targetActorName` (string) - имя целевого актора

**Пример:**
```lua
_SetCameraThirdPersonTarget("MainCamera", "Player")
```

---

### `_CreateFirstPersonCamera(cameraName, viewport, viewProjectionJson, initPitch, initYaw, initPosition, isMainCamera)`
Создает камеру от первого лица.

**Параметры:**
- `cameraName` (string) - имя камеры
- `viewport` (ivec4) - координаты вьюпорта {x, y, width, height}
- `viewProjectionJson` (string) - параметры проекции в JSON
- `initPitch` (float) - начальный угол наклона
- `initYaw` (float) - начальный угол поворота
- `initPosition` (vec3) - начальная позиция
- `isMainCamera` (integer) - 1 если основная камера

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
```
Создает камеру от первого лица.

**Параметры:**
- `cameraName` (string) - имя камеры
- `viewport` (ivec4) - координаты вьюпорта
- `viewProjectionJson` (string) - параметры проекции в JSON
- `initPitch` (float) - начальный угол наклона
- `initYaw` (float) - начальный угол поворота
- `initPosition` (vec3) - начальная позиция
- `isMainCamera` (integer) - 1 если основная камера

---

### `_CreateMaterial(materialJson)`
Создает экземпляр материала.

**Параметры:**
- `materialName` (string) - название материала

**Возвращает:** `integer` - ID Lua proxy материала

**Пример:**
```lua
local materialId = _CreateMaterial('materialName.m')
```

---

### `_SetTextureToMaterial(materialId, textureName, propertyName)`
Устанавливает текстуру в материал.

**Параметры:**
- `materialId` (integer) - ID материала
- `textureName` (string) - имя текстуры
- `propertyName` (string) - имя свойства материала

**Пример:**
```lua
_SetTextureToMaterial(materialId, "brick_diffuse.png", "diffuse")
```

---

### `_SetFloatToMaterial(materialId, value, propertyName)`
Устанавливает float значение в материал.

**Параметры:**
- `materialId` (integer) - ID материала
- `value` (float) - значение
- `propertyName` (string) - имя свойства

**Пример:**
```lua
_SetFloatToMaterial(materialId, 0.8, "metallic")
```

---

### `_SetVec3ToMaterial(materialId, propertyName, valueJson)`
Устанавливает vec3 значение в материал.

**Параметры:**
- `materialId` (integer) - ID материала
- `propertyName` (string) - имя свойства
- `valueJson` (string) - JSON со значением vec3

**Пример:**
```lua
_SetVec3ToMaterial(materialId, "albedo", '{"x": 1.0, "y": 0.5, "z": 0.2}')
```

---

### `_SetBindingToMaterial(materialId, gameObjectName, gamePropertyName, bindingName)`
Устанавливает привязку (binding) к свойству материала из игрового объекта.

**Параметры:**
- `materialId` (integer) - ID материала
- `gameObjectName` (string) - имя игрового объекта (например, "EngineScene")
- `gamePropertyName` (string) - имя свойства объекта (например, "GT_DeltaSec")
- `bindingName` (string) - имя привязки в материале (например, "deltaTimeSec")

**Пример:**
```lua
-- Привязка времени из сцены к материалу для анимированных шейдеров
_SetBindingToMaterial(skyboxMat, "EngineScene", "GT_DeltaSec", "deltaTimeSec")
```

**Применение:**
Эта функция позволяет динамически связывать свойства материала с игровыми объектами. Например:
- Передача времени для анимации шейдеров (вода, небо)
- Привязка параметров камеры
- Связь с параметрами света или других объектов сцены

---

### `_CreateTweener(actorId, tweenerJson)`
Создает твинер (анимацию) для актора.

**Параметры:**
- `actorId` (integer) - ID актора
- `tweenerJson` (string) - параметры твинера в JSON

**Возвращает:** `integer` - ID твинера

**Пример:**
```lua
local tweenerId = _CreateTweener(
    actorId,
    '{"duration": 2.0, "loop": true}'
)
```

---

### `_CreateActorController(controllerType, controllerName, actorName, jsonParams)`
Создает контроллер для актора.

**Параметры:**
- `controllerType` (string) - тип контроллера
- `controllerName` (string) - имя контроллера
- `actorName` (string) - имя актора
- `jsonParams` (string) - параметры в JSON

**Пример:**
```lua
_CreateActorController(
    "PlayerController",
    "Player1Controller",
    "PlayerActor",
    '{"speed": 5.0}'
)
```

---

## Функции событий

### `_SendPauseGameThreadEvent(enqueuePolicy, pauseState)`
Отправляет событие паузы/возобновления игры.

**Параметры:**
- `enqueuePolicy` (integer) - политика добавления в очередь
- `pauseState` (boolean) - true для паузы, false для возобновления

**Пример:**
```lua
_SendPauseGameThreadEvent(0, true)  -- пауза
```

---

## Игровые функции (GalaxyDefense)

### `_GetSelectedMissileType()`
Возвращает тип выбранной ракеты.

**Возвращает:** `integer` - тип ракеты (enum)

**Пример:**
```lua
local missileType = _GetSelectedMissileType()
```

---

### `_GetAllMissilesData()`
Возвращает данные о всех ракетах.

**Возвращает:** `string` - JSON строка с данными о ракетах

**Пример:**
```lua
local missilesData = _GetAllMissilesData()
local data = json.decode(missilesData)
```

---

### `_GetEnemySpaceshipsCountDestroyedByPlayer()`
Возвращает количество уничтоженных игроком вражеских кораблей.

**Возвращает:** `integer` - количество кораблей

**Пример:**
```lua
local destroyedCount = _GetEnemySpaceshipsCountDestroyedByPlayer()
print("Destroyed: " .. destroyedCount)
```

---

### `_GetEditorLevelAreaBoundingBoxWidth()`
Возвращает ширину области уровня в редакторе.

**Возвращает:** `float` - ширина

---

### `_GetEditorLevelAreaBoundingBoxLength()`
Возвращает длину области уровня в редакторе.

**Возвращает:** `float` - длина

---

### `_SendChangeGameModeGameThreadEvent(enqueuePolicy, gameModeType)`
Отправляет событие смены игрового режима.

**Параметры:**
- `enqueuePolicy` (integer) - политика добавления в очередь
- `gameModeType` (integer) - тип игрового режима (enum)

**Пример:**
```lua
_SendChangeGameModeGameThreadEvent(0, GameMode.Combat)
```

---

### `_SendChangeEditModeGameThreadEvent(enqueuePolicy, editModeType)`
Отправляет событие смены режима редактирования.

**Параметры:**
- `enqueuePolicy` (integer) - политика добавления в очередь
- `editModeType` (integer) - тип режима редактирования (enum)

---

## Lua Proxy объекты

Lua proxy объекты представляют C++ объекты в Lua коде и позволяют взаимодействовать с ними.

### Типы UI Proxy:
- **UiCanvasLuaProxy** - канвас (контейнер UI элементов)
- **UiLabelLuaProxy** - текстовая метка
- **UiImageLuaProxy** - изображение
- **UiButtonLuaProxy** - кнопка
- **UiToggleButtonLuaProxy** - переключатель
- **UiSliderLuaProxy** - слайдер
- **UiProgressBarLuaProxy** - прогресс-бар
- **UiTextBlockLuaProxy** - текстовый блок
- **UiRectangleLuaProxy** - прямоугольник
- **UiGridLayoutLuaProxy** - сеточный layout
- **UiRowLayoutLuaProxy** - строчный layout

### Общие методы UI Proxy:

Большинство UI proxy имеют следующие методы (вызываются через двоеточие):

```lua
-- Установка видимости
proxy:SetVisible(true)

-- Установка позиции (нормализованные координаты 0-1)
proxy:SetPosition(0.5, 0.5)

-- Установка размера
proxy:SetSize(200, 100)

-- Установка прозрачности
proxy:SetOpacity(0.8)

-- Установка Z-порядка
proxy:SetZOrder(10)

-- Получение имени
local name = proxy:GetName()
```

### Специфичные методы:

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

Предоставляет доступ к системе ввода:

```lua
local input = -- получается через LuaScriptProcessor

-- Клавиатура
local isPressedKeys = input:GetIsPressedKeyboardKeys()
local isReleasedKeys = input:GetIsReleasedKeyboardKeys()
local keyboardData = input:GetKeyboardJsonData()

-- Мышь
local mouseData = input:GetMouseJsonData()
```

---

## Примеры использования

### Создание UI элемента
```lua
-- Создание метки
local labelId = _CreateCommonUiWidget(
    UIWidgetType.Label,
    '{"name": "MyLabel", "x": 0.5, "y": 0.5, "text": "Score: 0"}'
)

-- Ожидание готовности proxy
while not _IsLuaProxyReady(labelId) do
    coroutine.yield()
end

-- Обновление текста
_OnCommonUiWidgetDataUpdated(labelId, '{"text": "Score: 100"}')
```

### Создание актора с компонентами
```lua
-- Загрузка ресурсов
_LazyLoadResourcesAsync("player_model.obj, player_texture.png")

-- Создание актора
local playerId = _CreateActor(
    "Actor",
    "Player",
    {0, 0, 0},
    {0, 0, 0},
    {1, 1, 1},
    '{}'
)

-- Добавление меша
_CreateAndAttachComponentToActor(
    playerId,
    "MeshComponent",
    '{"mesh": "player_model.obj"}'
)

-- Создание материала
local matId = _CreateMaterial('{"shader": "PBR"}')
_SetTextureToMaterial(matId, "player_texture.png", "diffuse")
```

### Анимация UI
```lua
local buttonId = _CreateCommonUiWidget(UIWidgetType.Button, '{}')

-- Добавление анимации
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

-- Запуск анимации
_StartUiItemAnimation(buttonId, "pulse")
```

---

## Enums и константы

### enqueueJobPolicy
- `0` - IF_DUPLICATE_NO_PUSH - не добавлять дубликаты
- `1` - IF_DUPLICATE_REPLACE - заменять дубликаты
- `2` - PUSH_ANYWAY - всегда добавлять

### UIWidgetType
- Определяется в `eCommonUiWidgetType`
- Canvas, Label, Image, Button, Slider, ProgressBar, etc.

### GameMode (GalaxyDefense)
- Зависит от реализации игры
- Combat, Edit, Menu, etc.

---

## Заметки

1. **Потоки**: PHEngine использует многопоточность. Lua выполняется в отдельном потоке, а некоторые операции (например, рендеринг) - в других потоках. Система автоматически синхронизирует данные между потоками.

2. **JSON параметры**: Многие функции принимают JSON строки для передачи сложных параметров. Используйте библиотеку `json` в Lua для работы с JSON.

3. **Proxy готовность**: После создания объектов через API нужно проверять готовность proxy с помощью `_IsLuaProxyReady()` перед использованием.

4. **Координаты UI**: UI использует нормализованные координаты (0.0 - 1.0) для позиционирования, где (0, 0) - левый верхний угол, (1, 1) - правый нижний.

5. **Ресурсы**: Всегда загружайте ресурсы перед использованием через `_LazyLoadResourcesAsync()`.

---

## Расширение API

Для добавления новых функций в Lua API:

1. Объявите функцию в соответствующем классе `LuaXxxFunctions.h`
2. Реализуйте функцию в `.cpp` файле
3. Зарегистрируйте через `LuaCallbackBindingHelper` в методе `RegisterCallbacks()`
4. Используйте префикс `_` для имени функции в Lua

Пример регистрации:
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
