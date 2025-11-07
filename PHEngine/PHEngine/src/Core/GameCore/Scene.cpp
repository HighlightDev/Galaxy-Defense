#include "Scene.h"

#include "Core/GameCore/Components/ComponentCreators/IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchHolder.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace Graphics;
using namespace TinyLogger;
using namespace EngineCore::DataProviders;
using namespace EngineCore::Scripts;
using namespace IO;

namespace EngineCore {

Scene::Scene(InterThreadCommunicationMgr& interThreadMgr)
    : EngineObject("EngineScene")
    , mPhysicsWorld(std::make_shared<PhysicsWorld>())
    , mEngineObjects()
    , mLuaReplicators()
    , m_interThreadMgr(interThreadMgr)
    , mGameThreadDeltaSec(std::make_shared<EngineObjectProperty<float>>(0.0f, "GT_DeltaSec"))
    , mScreenResolutionProperty(std::make_shared<EngineObjectProperty<glm::vec2>>(
          glm::vec2(
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight()),
          "ScreenResolution"))
    , mDeferredResourceCreators()
    , mActors()
    , mMainCamera()
    , mActiveCameras()
    , mActorControllers()
    , mMaterials()
    , mDynamicMaterials()
    , mInstancedGeometryBatchHolder(std::make_shared<InstancedGeometryBatchHolder>())
    ,
#ifdef DEBUG
    mDebugUiController(std::make_shared<DebugUiController>())
    ,
#endif
    mUiHandler(std::make_shared<UiHandler>())
{
    LogInfo("Scene::ctor");

    AddEngineProperty(mGameThreadDeltaSec);
    AddEngineProperty(mScreenResolutionProperty);
    mPhysicsWorld->Initialize();

#ifdef DEBUG
    mDebugUiController->Initialize();
#endif
}

Scene::~Scene()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->RemoveListener(WindowSizeChangedGameThreadEvent::GetInstanceId());
    MouseButtonDownRootEvent::GetInstance()->RemoveListener(MouseButtonDownRootEvent::GetInstanceId());
}

void Scene::Initialize()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
    MouseButtonDownRootEvent::GetInstance()->AddListener(shared_from_this());
}

void Scene::OnLevelInit()
{
    LogInfo("Scene::OnLevelInit");
    RegisterEngineObject(shared_from_this());

    MaterialParser materialParser;
    mOutlineMaterialSp = materialParser.ParseMaterialDescriptor("OutlineMaterial.m");
    MaterialPropertySetter::SetMaterialPropertyValue(mOutlineMaterialSp, "color", glm::vec3(0.8, 1.0, 0.2));
    RegisterMaterialInstance(mOutlineMaterialSp);

    mInstancedGeometryBatchHolder->SetScene(shared_from_this());
    mUiHandler->SetScene(shared_from_this());
#ifdef DEBUG
    mDebugUiController->SetScene(shared_from_this());
#endif
    mUiHandler->CreateHudCanvas(ViewPortInfo(
        0,
        0,
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight()));
}

void Scene::PostLevelInit()
{
    LogInfo("Scene::PostLevelInit");

    for (auto actor : mActors) {
        actor->SetScene(shared_from_this());
        actor->PostLevelInit();
    }

    for (auto camera : mActiveCameras) {
        camera->PostLevelInit();
    }

    mUiHandler->GetHudCanvas()->SetIsVisible(true);
}

void Scene::PostPhysicsInitialize()
{
    LogInfo("Scene::PostPhysicsInitialize");

    for (auto actor : mActors) {
        actor->PostPhysicsInitialize();
    }
}

void Scene::PostPlayLevelFinished()
{
    LogInfo("Scene::PostPlayLevelFinished");

    for (auto actor : mActors) {
        actor->PostPlayLevelFinished();
    }

    mDebugUiController->PostPlayLevelFinished();
}

void Scene::RegisterMainCamera(const std::shared_ptr<ACamera>& camera)
{
    LogInfo("Scene::RegisterMainCamera: name = ", camera->GetCameraName());

    assert(!mMainCamera);
    mMainCamera = camera;
    RegisterCamera(camera);
}

void Scene::RegisterCamera(const std::shared_ptr<ACamera>& camera)
{
    camera->Initialize();
    assert(!std::any_of(
        mActiveCameras.cbegin(), mActiveCameras.cend(), [cameraObjectId = camera->GetObjectId()](const auto& camera) {
            return camera->GetObjectId() == cameraObjectId;
        }));

    mActiveCameras.emplace_back(camera);
    RegisterEngineObject(camera);
    const auto cameraProxyPtr = camera->CreateSceneProxy();
    LogInfo("Scene::RegisterCamera: id = ", camera->GetObjectId(), ", cameraSceneProxyId: ", cameraProxyPtr->GetSceneProxyId());
    camera->SetCameraProxyId(cameraProxyPtr->GetSceneProxyId());

    if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
        sceneRendererSp->AddCameraSceneProxy_OnRenderThread(camera, cameraProxyPtr);
    }
}

void Scene::UnregisterCamera(const int32_t objectId)
{
    LogInfo("Scene::UnregisterCamera: id = ", objectId);
    auto foundCameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [objectId](const auto& camera) {
        return camera->GetObjectId() == objectId;
    });
    assert(foundCameraIt != mActiveCameras.end());
    const auto& cameraSp = *foundCameraIt;
    RemoveEngineObject(cameraSp->GetObjectId());

    if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
        sceneRendererSp->RemoveCameraSceneProxy_OnRenderThread(cameraSp->GetCameraProxyId());
        cameraSp->SetIsCameraProxyReady(false);
    }
    mActiveCameras.erase(foundCameraIt);
}

void Scene::UnregisterMainCamera()
{
    UnregisterCamera(mMainCamera->GetObjectId());
    mMainCamera.reset();
}

void Scene::UnregisterAllCameras()
{
    mActiveCameras.clear();
}

void Scene::RegisterMaterialInstance(const std::shared_ptr<IMaterial>& material)
{
    LogInfo("Scene::RegisterMaterialInstance: name = ", material->MaterialName);

    mMaterials.push_back(material);

    if (material->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC) {
        auto dynamicMaterial = std::static_pointer_cast<DynamicMaterial>(material);
        dynamicMaterial->SetScene(shared_from_this());
        mDynamicMaterials.push_back(dynamicMaterial);
    }

    const auto& materialProxy = material->CreateMaterialProxy();
    material->SetMaterialProxyWp(materialProxy);
    material->MaterialProxyId = materialProxy->GetSceneProxyId();

    if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
        sceneRendererSp->AddMaterialProxy_OnRenderThread(materialProxy);
    }
}

InterThreadCommunicationMgr& Scene::GetInterThreadCommunicationManager()
{
    return m_interThreadMgr;
}

std::shared_ptr<EnginePhysics::PhysicsWorld> Scene::GetPhysicsWorld() const
{
    return mPhysicsWorld;
}

std::shared_ptr<ACamera> Scene::GetCamera(const std::string& cameraName) const
{
    auto cameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [&](const auto& cameraPtr) {
        return cameraPtr->GetEngineObjectName() == cameraName;
    });

    if (cameraIt != mActiveCameras.end()) {
        return (*cameraIt);
    }

    return nullptr;
}

std::vector<std::shared_ptr<ACamera>> Scene::GetActiveCameras() const
{
    return mActiveCameras;
}

std::shared_ptr<ACamera> Scene::GetMainCamera() const
{
    assert(mMainCamera);
    return mMainCamera;
}

const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const
{
    return mActors;
}

std::shared_ptr<Actor> Scene::GetActorByName(const std::string& name) const
{
    auto foundActor
        = std::find_if(mActors.begin(), mActors.end(), [&](const auto& actor) { return actor->GetEngineObjectName() == name; });
    assert(foundActor != mActors.end());
    return *foundActor;
}

std::shared_ptr<Actor> Scene::GetActorById(const int32_t id) const
{
    auto foundActor = std::find_if(mActors.begin(), mActors.end(), [&](const auto& actor) { return actor->GetObjectId() == id; });
    assert(foundActor != mActors.end());
    return *foundActor;
}

std::shared_ptr<IMaterial> Scene::GetMaterialByProxyId(const size_t proxyId) const
{
    const auto materialIt = std::find_if(
        mMaterials.cbegin(), mMaterials.cend(), [=](const auto& material) { return material->MaterialProxyId == proxyId; });
    return materialIt != mMaterials.cend() ? *materialIt : nullptr;
}

std::shared_ptr<Graphics::IMaterial> Scene::GetMaterialByName(const std::string& materialName) const
{
    const auto materialIt = std::find_if(
        mMaterials.cbegin(), mMaterials.cend(), [=](const auto& material) { return material->MaterialName == materialName; });
    return materialIt != mMaterials.cend() ? *materialIt : nullptr;
}

void Scene::AddActor(std::shared_ptr<Actor> actor)
{
    RegisterEngineObject(actor);
    mActors.emplace_back(actor);
}

void Scene::RemoveActor(std::shared_ptr<Actor> actor)
{
    auto removeIt = std::remove_if(
        mEngineObjects.begin(), mEngineObjects.end(), [goName = actor->GetEngineObjectName()](const auto& gameObjectWp) {
            if (const auto& gameObjectSp = gameObjectWp.lock()) {
                return gameObjectSp->GetEngineObjectName() == goName;
            }
            return false;
        });

    mEngineObjects.erase(removeIt);
}

std::shared_ptr<EngineObject> Scene::GetEngineObjectByName(const std::string& name) const
{
    auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [=](const auto& gameObjectWp) {
        if (const auto& gameObjectSp = gameObjectWp.lock()) {
            return gameObjectSp->GetEngineObjectName() == name;
        }
        return false;
    });

    return it != mEngineObjects.end() ? it->lock() : nullptr;
}

std::shared_ptr<EngineObject> Scene::GetEngineObjectById(const int32_t id) const
{
    auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [=](const auto& gameObjectWp) {
        if (const auto& gameObjectSp = gameObjectWp.lock()) {
            return gameObjectSp->GetObjectId() == id;
        }
        return false;
    });

    return it != mEngineObjects.end() ? it->lock() : nullptr;
}

std::weak_ptr<IDeferredResourceCreator> Scene::GetDeferredResourceCreatorByName(const std::string& name) const
{
    if (mDeferredResourceCreators.count(name)) {
        return mDeferredResourceCreators.at(name);
    }

    LogInfo("Scene::GetDeferredResourceCreatorByName: missing resource creator with such name: ", name);
    return std::weak_ptr<IDeferredResourceCreator>();
}

const std::vector<std::shared_ptr<ActorController>>& Scene::GetActorControllers() const
{
    return mActorControllers;
}

void Scene::AddActorController(std::shared_ptr<ActorController> actorController)
{
    const auto it = std::find_if(mActorControllers.begin(), mActorControllers.end(), [&](const auto& existingController) {
        return existingController->GetBindedActorName() == actorController->GetBindedActorName();
    });
    assert(it == mActorControllers.end());

    actorController->Initialize();
    mActorControllers.emplace_back(actorController);
}

const std::shared_ptr<InstancedGeometryBatchHolder>& Scene::GetInstancedGeometryBatchHolder() const
{
    return mInstancedGeometryBatchHolder;
}

std::shared_ptr<UiHandler> Scene::GetUiHandler() const
{
    return mUiHandler;
}

void Scene::Tick(const float delta)
{
    mGameThreadDeltaSec->SetValue(delta);
    mScreenResolutionProperty->SetValue(glm::vec2(
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight()));

    mPhysicsWorld->Tick(delta);

    for (const auto cameraPtr : mActiveCameras) {
        cameraPtr->Tick(delta);
    }

    for (int i = 0; i < mActors.size(); ++i) {
        const auto& actor = mActors[i];
        if (actor && actor->IsEnabled()) {
            actor->Tick(delta);
        }
    }

    mInstancedGeometryBatchHolder->Tick(delta);

    for (const auto actorController : mActorControllers) {
        actorController->Tick(delta);
    }

    for (auto dynamicMaterial : mDynamicMaterials) {
        if (dynamicMaterial->IsEnabled()) {
            dynamicMaterial->Tick(delta);
        }
    }

#if DEBUG
    m_interThreadMgr.ExecuteOnRenderThread(
        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
        GetObjectId(),
        Hash("Scene::UpdatePhysicsRenderData"),
        [physRenderData = mPhysicsWorld->GetDebugPhysicsRenderData()](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                sceneRendererSp->SetDebugPhysicsRenderData(physRenderData);
            }
        });

    mDebugUiController->Tick(delta);
#endif

    mUiHandler->Tick(delta);
}

void Scene::UnpausableTick(const float deltaTimeSec)
{
    mPhysicsWorld->UnpausableTick(deltaTimeSec);

    for (const auto cameraPtr : mActiveCameras) {
        cameraPtr->UnpausableTick(deltaTimeSec);
    }

    for (auto actor : mActors) {
        actor->UnpausableTick(deltaTimeSec);
    }

    for (const auto actorController : mActorControllers) {
        actorController->UnpausableTick(deltaTimeSec);
    }

    for (auto dynamicMaterial : mDynamicMaterials) {
        dynamicMaterial->UnpausableTick(deltaTimeSec);
    }

#if DEBUG
    mDebugUiController->UnpausableTick(deltaTimeSec);
#endif

    if (mLuaReplicatorsDirty) {
        for (const auto& [id, replicator] : mLuaReplicators) {
            if (replicator->GetIsPendingToCreateLuaProxy()) {
                replicator->InitLuaProxy(shared_from_this());
            }
        }
        mLuaReplicatorsDirty = false;
    }

    mUiHandler->UnpausableTick(deltaTimeSec);
}

void Scene::ProcessEvent(
    const WindowSizeChangedGameThreadEvent* sender, const WindowSizeChangedGameThreadEvent::EventData_t& data)
{
    m_interThreadMgr.ExecuteOnRenderThread(
        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
        GetObjectId(),
        Hash("Scene::WindowSizeChangedGameThreadEvent"),
        [viewPortInfo = std::get<0>(data)](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                sceneRendererSp->OnWindowSizeChanged(viewPortInfo);
            }
        });
}

void Scene::ProcessEvent(const MouseButtonDownRootEvent* sender, const MouseButtonDownRootEvent::EventData_t& data)
{
    const auto& currentMousePosition = std::get<0>(data);
    const auto invertedScreenYPosition = static_cast<int32_t>(mScreenResolutionProperty->GetValue().y) - currentMousePosition.y;
    const std::vector<MouseKeysData>& mousePressedKeys = std::get<1>(data);

    const bool bContainsUnpressedKey
        = std::any_of(mousePressedKeys.cbegin(), mousePressedKeys.cend(), [](const MouseKeysData& keyData) {
              return keyData.State == KeyState::RELEASED;
          });

    std::vector<eMouseEventTargetReceiverType> receiverTypes;
    receiverTypes.reserve(2);
    if (bContainsUnpressedKey) {
        receiverTypes.push_back(eMouseEventTargetReceiverType::UI_INPUT_SYSTEM);
        receiverTypes.push_back(eMouseEventTargetReceiverType::SCENE_GAME_OBJECTS);
    } else {
        if (mUiHandler->CheckIfUiInterceptsMouseEvent(glm::ivec2(currentMousePosition.x, invertedScreenYPosition))) {
            receiverTypes.push_back(
                eMouseEventTargetReceiverType::UI_INPUT_SYSTEM); // Mouse press events will be propagated only to UI input system
        } else {
            receiverTypes.push_back(eMouseEventTargetReceiverType::SCENE_GAME_OBJECTS);
        }
    }

    for (const auto& receiverType : receiverTypes) {
        MouseButtonDownGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, receiverType, mousePressedKeys);
        MouseButtonDownLuaThreadEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, mousePressedKeys);
    }
}

void Scene::RemoveComponent(std::shared_ptr<Component> component)
{
    const eComponentType type = component->GetComponentType();

    // Remove corresponding primitive proxy
    if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT) {
        auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
        const size_t removeProxyIndex = componentPtr->GetSceneProxyId();

        // delete light proxy from render thread
        if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
            sceneRendererSp->RemovePrimitiveSceneProxy_OnRenderThread(removeProxyIndex);
        }
    } else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT) {
        auto componentPtr = std::static_pointer_cast<LightComponent>(component);
        const size_t removeProxyIndex = componentPtr->GetLightSceneProxyId();

        // delete light proxy from render thread
        if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
            sceneRendererSp->DeleteLightSceneProxy_OnRenderThread(removeProxyIndex);
        }
    }

    if (const auto& spOwner = component->GetOwner().lock()) {
        if ((type & eComponentType::MOVEMENT_COMPONENT) == eComponentType::MOVEMENT_COMPONENT) {
            spOwner->RemoveMovementComponent();
        } else if ((type & eComponentType::INPUT_COMPONENT) == eComponentType::INPUT_COMPONENT) {
            spOwner->RemoveInputComponent();
        } else {
            spOwner->RemoveComponent(component);
        }
    }

    RemoveEngineObject(component->GetObjectId());
}

void Scene::RegisterComponentSceneProxy(const std::shared_ptr<Component>& componentSp)
{
    LogInfo("Scene::RegisterComponentSceneProxy: componentName = ", componentSp->GetEngineObjectName());

    const eComponentType type = componentSp->GetComponentType();
    if ((type & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT) {
        if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT) {
            const auto primitiveComponentSp = std::static_pointer_cast<PrimitiveComponent>(componentSp);
            const auto sceneProxySp = primitiveComponentSp->CreateSceneProxy();
            sceneProxySp->SetDeferredShadingSceneRenderer(m_interThreadMgr.GetSceneRendererWP());
            primitiveComponentSp->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            sceneProxySp->SetBindedGameObjectId(primitiveComponentSp->GetObjectId());
            if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
                sceneRendererSp->AddPrimitiveSceneProxy_OnRenderThread(primitiveComponentSp, sceneProxySp);
            }
        } else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT) {
            const auto lightComponentSp = std::static_pointer_cast<LightComponent>(componentSp);
            const auto sceneProxySp = lightComponentSp->CreateSceneProxy();
            lightComponentSp->SetLightSceneProxyId(sceneProxySp->GetSceneProxyId());
            if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
                sceneRendererSp->AddLightSceneProxy_OnRenderThread(lightComponentSp, sceneProxySp);
            }
        } else if ((type & eComponentType::PLANAR_REFLECTION_COMPONENT) == eComponentType::PLANAR_REFLECTION_COMPONENT) {
            const auto planarComponentSp = std::static_pointer_cast<PlanarReflectionComponent>(componentSp);
            const auto sceneProxySp = planarComponentSp->CreatePlanarReflectionProxy();
            planarComponentSp->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            if (const auto& sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock()) {
                const auto ownerCameraSp = planarComponentSp->GetOwnerCameraWp().lock();
                assert(ownerCameraSp);
                sceneRendererSp->BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(
                    sceneProxySp, ownerCameraSp->GetCameraProxyId());
                sceneRendererSp->AddPlanarReflectionSceneProxy_OnRenderThread(planarComponentSp, sceneProxySp);
            }
        } else {
            LogInfo("Scene::RegisterComponentSceneProxy: Warning: unsupported scene component type");
        }
    }
}

std::shared_ptr<Component> Scene::CreateComponent_GameThread(
    const std::shared_ptr<IComponentCreatable>& componentCreator, const std::shared_ptr<ComponentData>& componentData)
{
    const auto component = componentCreator->CreateComponent(shared_from_this(), componentData);
    component->Initialize();
    component->SetScene(shared_from_this());
    RegisterComponentSceneProxy(component);
    RegisterEngineObject(component);
    component->OnPostInitialized();
    return component;
}

bool Scene::RegisterDeferredResourceCreator(
    const std::shared_ptr<IDeferredResourceCreator>& creatorInstance, const std::string& gameObjectName)
{
    // Add deferred resource creator instance
    assert(!mDeferredResourceCreators.count(gameObjectName));
    mDeferredResourceCreators[gameObjectName] = creatorInstance;

    return true;
}

bool Scene::RemoveDeferredResourceCreator(const std::string& gameObjectName)
{
    // Remove deferred resource creator instance
    if (mDeferredResourceCreators.count(gameObjectName)) {
        mDeferredResourceCreators.erase(gameObjectName);
        return true;
    }

    return false;
}

bool Scene::RegisterEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase>& replicator)
{
    const auto replicatorId = replicator->GetReplicatorId();
    if (!mLuaReplicators.count(replicatorId)) {
        mLuaReplicators.emplace(std::make_pair(replicatorId, replicator));
        mLuaReplicatorsDirty = true;
        return true;
    }

    return false;
}

bool Scene::UnregisterEngineToLuaReplicator(const int32_t replicatorId)
{
    if (mLuaReplicators.count(replicatorId)) {
        mLuaReplicators.erase(replicatorId);
        return true;
    }

    return false;
}

std::shared_ptr<EngineToLuaReplicatorBase> Scene::GetEngineToLuaReplicatorById(const int32_t id) const
{
    if (mLuaReplicators.count(id)) {
        return mLuaReplicators.at(id);
    }

    return nullptr;
}

std::shared_ptr<EngineToLuaReplicatorBase> Scene::GetEngineToLuaReplicatorByLuaProxyId(const int32_t id) const
{
    auto it = std::find_if(mLuaReplicators.cbegin(), mLuaReplicators.cend(), [id](const auto& luaReplicatorPair) {
        return luaReplicatorPair.second->GetLuaProxyId() == id;
    });

    return (it != mLuaReplicators.cend()) ? it->second : nullptr;
}

void Scene::RegisterEngineObject(const std::shared_ptr<EngineObject>& gameObject)
{
    const auto it = std::find_if(
        mEngineObjects.cbegin(), mEngineObjects.cend(), [objectId = gameObject->GetObjectId()](const auto& gameObjectWp) {
            if (const auto& gameObjectSp = gameObjectWp.lock()) {
                return gameObjectSp->GetObjectId() == objectId;
            }
            return false;
        });

    if (it == mEngineObjects.cend()) {
        mEngineObjects.emplace_back(gameObject);
    }
}

void Scene::RemoveEngineObject(const uint32_t objectId)
{
    mEngineObjects.erase(std::remove_if(mEngineObjects.begin(), mEngineObjects.end(), [objectId](const auto& gameObjectWp) {
        if (const auto& gameObjectSp = gameObjectWp.lock()) {
            return gameObjectSp->GetObjectId() == objectId;
        }
        return false;
    }));
}

glm::vec4 Scene::GetConvertedToClippedSpacePosition(const size_t cameraProxyId, const glm::vec4& worldPosition)
{
    glm::vec4 result = worldPosition;

    if (const auto& sceneRenderer = m_interThreadMgr.GetSceneRendererWP().lock()) {
        const auto& sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
        if (sceneViewSp) {
            const auto& cameraProxySp = sceneViewSp->GetCameraProxy();
            const auto& viewMatrix = cameraProxySp->GetViewMatrix();
            const auto& projectionMatrix = cameraProxySp->GetProjectionMatrix();
            result = projectionMatrix * viewMatrix * result;
        } else {
            LogInfo(
                "Scene::GetConvertedToClippedSpacePosition: "
                "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                cameraProxyId);
        }
    }

    return result;
}

glm::vec3 Scene::GetConvertedToNDCSpacePosition(const size_t cameraProxyId, const glm::vec4& worldPosition)
{
    const glm::vec4 clippedSpacePosition = GetConvertedToClippedSpacePosition(cameraProxyId, worldPosition);
    if (EngineMath::FloatsNearEqual(clippedSpacePosition.w, 0.0f)) {
        LogInfo(
            "Scene::GetConvertedToNDCSpacePosition: "
            "Error! W is equal to zero. Potential zero division!",
            cameraProxyId);
        return glm::vec3();
    }
    const glm::vec3 ndcPosition = glm::vec3(clippedSpacePosition) / glm::vec3(clippedSpacePosition.w);
    return ndcPosition;
}

glm::vec2 Scene::GetConvertedToTextureSpacePosition(const size_t cameraProxyId, const glm::vec4& worldPosition)
{
    const glm::vec3 ndc = GetConvertedToNDCSpacePosition(cameraProxyId, worldPosition);
    return (glm::vec2(ndc) * glm::vec2(0.5)) + glm::vec2(0.5);
}

std::optional<CameraFrustum> Scene::GetCameraFrustum(const size_t cameraProxyId)
{
    std::optional<CameraFrustum> result(std::nullopt);

    if (const auto& sceneRenderer = m_interThreadMgr.GetSceneRendererWP().lock()) {
        const auto& sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
        if (sceneViewSp) {
            if (sceneViewSp->GetCameraProxy()->IsCameraFrustumBuilt()) {
                result = sceneViewSp->GetCameraProxy()->GetCameraFrustum();
            } else {
                LogInfo(
                    "Scene::GetCameraFrustum: "
                    "Error! Camera Frustum wasn't built yet. Proxy index = ",
                    cameraProxyId);
            }
        } else {
            LogInfo(
                "Scene::GetCameraFrustum: "
                "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                cameraProxyId);
        }
    }

    return result;
}

void Scene::UnloadScene()
{
    UnloadUi();
    UnregisterMainCamera();
    UnregisterAllCameras();
    UnloadPhysics();
    UnloadActors();
    UnloadEngineObjects();
    UnloadLuaReplicators();
    UnloadDeferredResourceCreators();
    UnloadActorControllers();
    UnloadMaterials();
}

void Scene::UnloadUi()
{
    mUiHandler->CleanUp();
    mInstancedGeometryBatchHolder->CleanUp();
}

void Scene::UnloadActors()
{
    for (const auto actor : mActors) {
        actor->CleanUp();
    }
    mActors.clear();
}

void Scene::UnloadPhysics()
{
    mPhysicsWorld->UnloadExistingPhysicsSimulation();
    mPhysicsWorld.reset();
    mPhysicsWorld = std::make_shared<PhysicsWorld>();
    mPhysicsWorld->Initialize();
}

void Scene::UnloadEngineObjects()
{
    mEngineObjects.clear();
}

void Scene::UnloadLuaReplicators()
{
    mLuaReplicators.clear();
    mLuaReplicatorsDirty = false;
}

void Scene::UnloadDeferredResourceCreators()
{
    mDeferredResourceCreators.clear();
}

void Scene::UnloadActorControllers()
{
    for (const auto actorController : mActorControllers) {
        actorController->CleanUp();
    }
    mActorControllers.clear();
}

void Scene::UnloadMaterials()
{
    mDynamicMaterials.clear();

    for (const auto materialInstance : mMaterials) {
        materialInstance->CleanUp();
    }
    mMaterials.clear();
}

#ifdef DEBUG

void Scene::SetRenderThreadFPSTextValue(const float fps)
{
    if (mDebugUiController) {
        const auto value = std::to_string(fps);
        mDebugUiController->SetRenderFpsText(value.substr(0, IndexOf(value, ".") + 2));
    }
}

void Scene::SetGameThreadFPSTextValue(const float fps)
{
    if (mDebugUiController) {
        const auto value = std::to_string(fps);
        mDebugUiController->SetGameFpsText(value.substr(0, IndexOf(value, ".") + 2));
    }
}

void Scene::SetLuaThreadFPSTextValue(const float fps)
{
    if (mDebugUiController) {
        const auto value = std::to_string(fps);
        mDebugUiController->SetLuaFpsText(value.substr(0, IndexOf(value, ".") + 2));
    }
}

#endif

std::shared_ptr<IMaterial> Scene::GetOutlineMaterial() const
{
    return mOutlineMaterialSp;
}
std::shared_ptr<EngineCore::GUI::UiCanvas> Scene::GetHudCanvas() const
{
    return mUiHandler->GetHudCanvas();
}
} // namespace EngineCore
