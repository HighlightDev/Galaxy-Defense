#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GameCore/GUI/Common/TextFieldProxy.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentCreators/IComponentCreatable.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"

using namespace Graphics;
using namespace TinyLogger;
using namespace EngineCore::Scripts;
using namespace IO;

namespace EngineCore
{

   Scene::Scene(InterThreadCommunicationMgr &interThreadMgr)
       : EngineObject("EngineScene"),
         mPhysicsWorld(std::make_shared<PhysicsWorld>()),
         mEngineObjects(),
         mLuaReplicators(),
         m_interThreadMgr(interThreadMgr),
         mGameThreadDeltaSec(std::make_shared<EngineObjectProperty<float>>(0.0f, "GT_DeltaSec")),
         mScreenResolutionProperty(std::make_shared<EngineObjectProperty<glm::vec2>>(glm::vec2(DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                                               DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()),
                                                                                     "ScreenResolution")),
         mDeferredResourceCreators(),
         mActors(),
         mMainCamera(),
         mActiveCameras(),
         mActorControllers(),
         mMaterials(),
         mDynamicMaterials(),
         mTextHandler(std::make_shared<TextHandler>()),
#ifdef DEBUG
         mDebugUiController(std::make_unique<DebugUiController>()),
#endif
         mUiHandler(std::make_shared<UiHandler>())
   {
      LogInfo("Scene::ctor");

      mTextHandler->Initialize();
      AddEngineProperty(mGameThreadDeltaSec);
      AddEngineProperty(mScreenResolutionProperty);
      mPhysicsWorld->Initialize();
   }

   void Scene::OnLevelInit()
   {
      LogInfo("Scene::OnLevelInit");
      RegisterEngineObject(shared_from_this());
   }

   void Scene::PostLevelInit()
   {
      LogInfo("Scene::PostLevelInit");

      mTextHandler->SetScene(shared_from_this());
      mUiHandler->SetScene(shared_from_this());
#ifdef DEBUG
      mDebugUiController->SetScene(shared_from_this());
#endif

      for (auto &actor : mActors)
      {
         actor->SetScene(shared_from_this());
         actor->PostLevelInit();
      }

      for (auto &camera : mActiveCameras)
      {
         camera->PostLevelInit();
      }
   }

   void Scene::PostPhysicsInitialize()
   {
      LogInfo("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::PostPlayLevelFinished()
   {
      LogInfo("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPlayLevelFinished();
      }

      mDebugUiController->PostPlayLevelFinished();
   }

   void Scene::RegisterMainCamera(const std::shared_ptr<ACamera> &camera)
   {
      LogInfo("Scene::RegisterMainCamera => name = ", camera->GetCameraName());

      assert(!mMainCamera);
      mMainCamera = camera;
      RegisterCamera(camera);
   }

   void Scene::RegisterCamera(const std::shared_ptr<ACamera> &camera)
   {
      camera->Initialize();
      assert(!std::any_of(mActiveCameras.cbegin(), mActiveCameras.cend(), [cameraObjectId = camera->GetObjectId()](const auto &camera)
                          { return camera->GetObjectId() == cameraObjectId; }));

      mActiveCameras.emplace_back(camera);
      RegisterEngineObject(camera);
      const auto cameraProxyPtr = camera->CreateSceneProxy();
      LogInfo("Scene::RegisterCamera => id = ", camera->GetObjectId(), ", cameraSceneProxyId: ", cameraProxyPtr->GetSceneProxyId());
      camera->SetCameraProxyId(cameraProxyPtr->GetSceneProxyId());

      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         sceneRendererSp->CameraSceneProxyAdded_OnRenderThread(camera, cameraProxyPtr);
      }
   }

   void Scene::UnregisterCamera(const int32_t objectId)
   {
      LogInfo("Scene::UnregisterCamera => id = ", objectId);
      auto foundCameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [objectId](const auto &camera)
                                        { return camera->GetObjectId() == objectId; });
      assert(foundCameraIt != mActiveCameras.end());
      const auto &cameraSp = *foundCameraIt;
      RemoveEngineObject(cameraSp->GetObjectId());

      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
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

   void Scene::RegisterMaterialInstance(const std::shared_ptr<IMaterial> &material)
   {
      LogInfo("Scene::RegisterMaterialInstance => name = ", material->MaterialName);

      mMaterials.push_back(material);

      if (material->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC)
      {
         auto dynamicMaterial = std::static_pointer_cast<DynamicMaterial>(material);
         dynamicMaterial->SetScene(shared_from_this());
         mDynamicMaterials.push_back(dynamicMaterial);
      }

      const auto &materialProxy = material->CreateMaterialProxy();
      material->SetMaterialProxyWp(materialProxy);
      material->MaterialProxyId = materialProxy->GetSceneProxyId();

      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         sceneRendererSp->MaterialProxyAdded_OnRenderThread(materialProxy);
      }
   }

   std::shared_ptr<IMaterial> Scene::GetMaterialInstanceById(const int32_t materialProxyId) const
   {
      const auto foundIt = std::find_if(mMaterials.cbegin(), mMaterials.cend(), [materialProxyId](const auto &instance)
                                        { return instance->MaterialProxyId == materialProxyId; });
      return foundIt != mMaterials.cend() ? *foundIt : nullptr;
   }

   InterThreadCommunicationMgr &Scene::GetInterThreadCommunicationManager()
   {
      return m_interThreadMgr;
   }

   std::shared_ptr<EnginePhysics::PhysicsWorld> Scene::GetPhysicsWorld() const
   {
      return mPhysicsWorld;
   }

   std::shared_ptr<ACamera> Scene::GetCamera(const std::string &cameraName) const
   {
      auto cameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [&](const auto &cameraPtr)
                                   { return cameraPtr->GetEngineObjectName() == cameraName; });

      if (cameraIt != mActiveCameras.end())
      {
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

   const std::vector<std::shared_ptr<Actor>> &Scene::GetActors() const
   {
      return mActors;
   }

   std::shared_ptr<Actor> Scene::GetActorByName(const std::string &name) const
   {
      auto foundActor = std::find_if(mActors.begin(), mActors.end(), [&](const auto &actor)
                                     { return actor->GetEngineObjectName() == name; });
      assert(foundActor != mActors.end());
      return *foundActor;
   }

   std::shared_ptr<Actor> Scene::GetActorById(const int32_t id) const
   {
      auto foundActor = std::find_if(mActors.begin(), mActors.end(), [&](const auto &actor)
                                     { return actor->GetObjectId() == id; });
      assert(foundActor != mActors.end());
      return *foundActor;
   }

   std::shared_ptr<IMaterial> Scene::GetMaterialByProxyId(const size_t proxyId) const
   {
      const auto materialIt = std::find_if(mMaterials.begin(), mMaterials.end(), [=](const auto &material)
                                           { return material->MaterialProxyId == proxyId; });
      return materialIt != mMaterials.end() ? *materialIt : nullptr;
   }

   void Scene::AddActor(std::shared_ptr<Actor> actor)
   {
      RegisterEngineObject(actor);
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      auto removeIt = std::remove_if(mEngineObjects.begin(), mEngineObjects.end(), [goName = actor->GetEngineObjectName()](const auto &gameObjectWp)
                                     { 
                                 if (const auto& gameObjectSp = gameObjectWp.lock())
                                 {
                                    return gameObjectSp->GetEngineObjectName() == goName;
                                 }
                                 return false; });

      mEngineObjects.erase(removeIt);
   }

   std::shared_ptr<EngineObject> Scene::GetEngineObjectByName(const std::string &name) const
   {
      auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [=](const auto &gameObjectWp)
                             { 
                              if (const auto& gameObjectSp = gameObjectWp.lock())
                              {
                                 return gameObjectSp->GetEngineObjectName() == name;
                              }
                              return false; });

      return it != mEngineObjects.end() ? it->lock() : nullptr;
   }

   std::shared_ptr<EngineObject> Scene::GetEngineObjectById(const int32_t id) const
   {
      auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [=](const auto &gameObjectWp)
                             { 
                              if (const auto& gameObjectSp = gameObjectWp.lock())
                              {
                                 return gameObjectSp->GetObjectId() == id;
                              }
                              return false; });

      return it != mEngineObjects.end() ? it->lock() : nullptr;
   }

   std::weak_ptr<IDeferredResourceCreator> Scene::GetDeferredResourceCreatorByName(const std::string &name) const
   {
      if (mDeferredResourceCreators.count(name))
      {
         return mDeferredResourceCreators.at(name);
      }

      LogInfo("Scene::GetDeferredResourceCreatorByName => missing resource creator with such name: ", name);
      return std::weak_ptr<IDeferredResourceCreator>();
   }

   const std::vector<std::shared_ptr<ActorController>> &Scene::GetActorControllers() const
   {
      return mActorControllers;
   }

   void Scene::AddActorController(std::shared_ptr<ActorController> actorController)
   {
      const auto it = std::find_if(mActorControllers.begin(), mActorControllers.end(), [&](const auto &existingController)
                                   { return existingController->GetBindedActorName() == actorController->GetBindedActorName(); });
      assert(it == mActorControllers.end());

      actorController->Initialize();
      mActorControllers.emplace_back(actorController);
   }

   const std::shared_ptr<TextHandler> &Scene::GetTextHandler() const
   {
      return mTextHandler;
   }

   std::shared_ptr<UiHandler> Scene::GetUiHandler() const
   {
      return mUiHandler;
   }

   void Scene::Tick(const float delta)
   {
      mGameThreadDeltaSec->SetValue(delta);
      mScreenResolutionProperty->SetValue(glm::vec2(DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                    DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()));

      mPhysicsWorld->Tick(delta);

      for (const auto &cameraPtr : mActiveCameras)
      {
         cameraPtr->Tick(delta);
      }

      for (auto &actor : mActors)
      {
         if (actor->IsEnabled())
         {
            actor->Tick(delta);
         }
      }

      for (const auto &actorController : mActorControllers)
      {
         actorController->Tick(delta);
      }

      for (auto &dynamicMaterial : mDynamicMaterials)
      {
         if (dynamicMaterial->IsEnabled())
         {
            dynamicMaterial->Tick(delta);
         }
      }

#if DEBUG
      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(),
                                                Hash("Scene::UpdatePhysicsRenderData"), [sceneRendererSp, physRenderData = mPhysicsWorld->GetDebugPhysicsRenderData()]()
                                                { sceneRendererSp->SetDebugPhysicsRenderData(physRenderData); });
      }

      mDebugUiController->Tick(delta);
#endif

      mUiHandler->Tick(delta);
   }

   void Scene::UnpausableTick(const float deltaTime)
   {
      mPhysicsWorld->UnpausableTick(deltaTime);

      for (const auto &cameraPtr : mActiveCameras)
      {
         cameraPtr->UnpausableTick(deltaTime);
      }

      for (auto &actor : mActors)
      {
         actor->UnpausableTick(deltaTime);
      }

      for (const auto &actorController : mActorControllers)
      {
         actorController->UnpausableTick(deltaTime);
      }

      for (auto &dynamicMaterial : mDynamicMaterials)
      {
         dynamicMaterial->UnpausableTick(deltaTime);
      }

#if DEBUG
      mDebugUiController->UnpausableTick(deltaTime);
#endif

      if (mLuaReplicatorsDirty)
      {
         for (const auto &[id, replicator] : mLuaReplicators)
         {
            if (replicator->GetIsPendingToCreateLuaProxy())
            {
               replicator->InitLuaProxy(shared_from_this());
            }
         }
         mLuaReplicatorsDirty = false;
      }

      mUiHandler->UnpausableTick(deltaTime);
   }

   void Scene::RemoveComponent(std::shared_ptr<Component> component)
   {
      const eComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
         const size_t removeProxyIndex = componentPtr->GetSceneProxyId();

         // delete light proxy from render thread
         if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
         {
            sceneRendererSp->PrimitiveSceneProxyDeleted_OnRenderThread(removeProxyIndex);
         }
      }
      else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<LightComponent>(component);
         const size_t removeProxyIndex = componentPtr->GetLightSceneProxyId();

         // delete light proxy from render thread
         if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
         {
            sceneRendererSp->LightSceneProxyDeleted_OnRenderThread(removeProxyIndex);
         }
      }

      if (const auto &spOwner = component->GetOwner().lock())
      {
         if ((type & eComponentType::MOVEMENT_COMPONENT) == eComponentType::MOVEMENT_COMPONENT)
         {
            spOwner->RemoveMovementComponent();
         }
         else if ((type & eComponentType::INPUT_COMPONENT) == eComponentType::INPUT_COMPONENT)
         {
            spOwner->RemoveInputComponent();
         }
         else
         {
            spOwner->RemoveComponent(component);
         }
      }

      RemoveEngineObject(component->GetObjectId());
   }

   void Scene::RegisterComponentSceneProxy(const std::shared_ptr<Component> &componentSp)
   {
      LogInfo("Scene::RegisterComponentSceneProxy => componentName = ", componentSp->GetEngineObjectName());

      const eComponentType type = componentSp->GetComponentType();
      if ((type & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
      {
         if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
         {
            const auto primitiveComponentSp = std::static_pointer_cast<PrimitiveComponent>(componentSp);
            const auto sceneProxySp = primitiveComponentSp->CreateSceneProxy();
            sceneProxySp->SetDeferredShadingSceneRenderer(m_interThreadMgr.GetSceneRendererWP());
            primitiveComponentSp->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            sceneProxySp->SetBindedGameObjectId(primitiveComponentSp->GetObjectId());
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               sceneRendererSp->PrimitiveSceneProxyAdded_OnRenderThread(primitiveComponentSp, sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => primitive proxy added, sceneProxyId =", primitiveComponentSp->GetSceneProxyId());
         }
         else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
         {
            const auto lightComponentSp = std::static_pointer_cast<LightComponent>(componentSp);
            const auto sceneProxySp = lightComponentSp->CreateSceneProxy();
            lightComponentSp->SetLightSceneProxyId(sceneProxySp->GetSceneProxyId());
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               sceneRendererSp->LightSceneProxyAdded_OnRenderThread(lightComponentSp, sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => ligth proxy added, sceneProxyId =", lightComponentSp->GetLightSceneProxyId());
         }
         else if ((type & eComponentType::PLANAR_REFLECTION_COMPONENT) == eComponentType::PLANAR_REFLECTION_COMPONENT)
         {
            const auto planarComponentSp = std::static_pointer_cast<PlanarReflectionComponent>(componentSp);
            const auto sceneProxySp = planarComponentSp->CreatePlanarReflectionProxy();
            planarComponentSp->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               const auto ownerCameraSp = planarComponentSp->GetOwnerCameraWp().lock();
               assert(ownerCameraSp);
               sceneRendererSp->BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(sceneProxySp, ownerCameraSp->GetCameraProxyId());
               sceneRendererSp->PlanarReflectionSceneProxyAdded_OnRenderThread(planarComponentSp, sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => planar reflection proxy added, sceneProxyId =", planarComponentSp->GetSceneProxyId());
         }
      }
   }

   std::shared_ptr<Component> Scene::CreateComponent_GameThread(const std::shared_ptr<IComponentCreatable> &componentCreator,
                                                                const std::shared_ptr<ComponentData> &componentData)
   {
      const auto component = componentCreator->CreateComponent(shared_from_this(), componentData);
      component->Initialize();
      component->SetScene(shared_from_this());
      RegisterComponentSceneProxy(component);
      RegisterEngineObject(component);
      component->OnPostInitialized();
      return component;
   }

   bool Scene::RegisterDeferredResourceCreator(const std::shared_ptr<IDeferredResourceCreator> &creatorInstance, const std::string &gameObjectName)
   {
      // Add deferred resource creator instance
      assert(!mDeferredResourceCreators.count(gameObjectName));
      mDeferredResourceCreators[gameObjectName] = creatorInstance;

      return true;
   }

   bool Scene::RemoveDeferredResourceCreator(const std::string &gameObjectName)
   {
      // Remove deferred resource creator instance
      if (mDeferredResourceCreators.count(gameObjectName))
      {
         mDeferredResourceCreators.erase(gameObjectName);
         return true;
      }

      return false;
   }

   bool Scene::RegisterEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase> &replicator)
   {
      const auto replicatorId = replicator->GetReplicatorId();
      if (!mLuaReplicators.count(replicatorId))
      {
         mLuaReplicators.emplace(std::make_pair(replicatorId, replicator));
         mLuaReplicatorsDirty = true;
         return true;
      }

      return false;
   }

   bool Scene::UnregisterEngineToLuaReplicator(const int32_t replicatorId)
   {
      if (mLuaReplicators.count(replicatorId))
      {
         mLuaReplicators.erase(replicatorId);
         return true;
      }

      return false;
   }

   std::shared_ptr<EngineToLuaReplicatorBase> Scene::GetEngineToLuaReplicatorById(const int32_t id) const
   {
      if (mLuaReplicators.count(id))
      {
         return mLuaReplicators.at(id);
      }

      return nullptr;
   }

   std::shared_ptr<EngineToLuaReplicatorBase> Scene::GetEngineToLuaReplicatorByLuaProxyId(const int32_t id) const
   {
      auto it = std::find_if(mLuaReplicators.cbegin(), mLuaReplicators.cend(), [id](const auto &luaReplicatorPair)
                             { return luaReplicatorPair.second->GetLuaProxyId() == id; });

      return (it != mLuaReplicators.cend()) ? it->second : nullptr;
   }

   void Scene::RegisterEngineObject(const std::shared_ptr<EngineObject> &gameObject)
   {
      const auto it = std::find_if(mEngineObjects.cbegin(), mEngineObjects.cend(), [objectId = gameObject->GetObjectId()](const auto &gameObjectWp)
                                   {
                              if (const auto& gameObjectSp = gameObjectWp.lock()) {
                                 return gameObjectSp->GetObjectId() == objectId; 
                              }
                              return false; });

      if (it == mEngineObjects.cend())
      {
         mEngineObjects.emplace_back(gameObject);
      }
   }

   void Scene::RemoveEngineObject(const uint32_t objectId)
   {
      mEngineObjects.erase(std::remove_if(mEngineObjects.begin(), mEngineObjects.end(), [objectId](const auto &gameObjectWp)
                                          {
                                             if (const auto& gameObjectSp = gameObjectWp.lock()) {
                                                return gameObjectSp->GetObjectId() == objectId; 
                                             }
                                             return false; }));
   }

   glm::vec4 Scene::GetConvertedToClippedSpacePosition(const size_t cameraProxyId, const glm::vec4 &worldPosition)
   {
      glm::vec4 result = worldPosition;

      if (const auto &sceneRenderer = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
         if (sceneViewSp)
         {
            const auto &cameraProxySp = sceneViewSp->GetCameraProxy();
            const auto &viewMatrix = cameraProxySp->GetViewMatrix();
            const auto &projectionMatrix = cameraProxySp->GetProjectionMatrix();
            result = projectionMatrix * viewMatrix * result;
         }
         else
         {
            LogInfo("Scene::GetConvertedToClippedSpacePosition => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    cameraProxyId);
         }
      }

      return result;
   }

   std::optional<CameraFrustum> Scene::GetCameraFrustum(const size_t cameraProxyId)
   {
      std::optional<CameraFrustum> result(std::nullopt);

      if (const auto &sceneRenderer = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
         if (sceneViewSp)
         {
            if (sceneViewSp->GetCameraProxy()->IsCameraFrustumBuilt())
            {
               result = sceneViewSp->GetCameraProxy()->GetCameraFrustum();
            }
            else
            {
               LogInfo("Scene::GetCameraFrustum => "
                       "Error! Camera Frustum wasn't built yet. Proxy index = ",
                       cameraProxyId);
            }
         }
         else
         {
            LogInfo("Scene::GetCameraFrustum => "
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
   }

   void Scene::UnloadActors()
   {
      for (const auto &actor : mActors)
      {
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
      for (const auto &actorController : mActorControllers)
      {
         actorController->CleanUp();
      }
      mActorControllers.clear();
   }

   void Scene::UnloadMaterials()
   {
      mDynamicMaterials.clear();

      for (const auto &materialInstance : mMaterials)
      {
         materialInstance->CleanUp();
      }
      mMaterials.clear();
   }

#ifdef DEBUG

   void Scene::SetRenderThreadFPSTextValue(const float fps)
   {
      if (mDebugUiController)
      {
         const auto value = std::to_string(fps);
         mDebugUiController->SetRenderFpsText(value.substr(0, IndexOf(value, ".") + 2));
      }
   }

   void Scene::SetGameThreadFPSTextValue(const float fps)
   {
      if (mDebugUiController)
      {
         const auto value = std::to_string(fps);
         mDebugUiController->SetGameFpsText(value.substr(0, IndexOf(value, ".") + 2));
      }
   }

   void Scene::SetLuaThreadFPSTextValue(const float fps)
   {
      if (mDebugUiController)
      {
         const auto value = std::to_string(fps);
         mDebugUiController->SetLuaFpsText(value.substr(0, IndexOf(value, ".") + 2));
      }
   }

#endif
}
