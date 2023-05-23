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

using namespace Graphics;
using namespace TinyLogger;
using namespace EngineCore::Scripts;

namespace EngineCore
{

   Scene::Scene(InterThreadCommunicationMgr &interThreadMgr)
       : EngineObject("EngineScene"),
         mPhysicsWorld(new PhysicsWorld()),
         mEngineObjects(),
         mLuaReplicators(),
         m_interThreadMgr(interThreadMgr),
         mGameThreadDeltaSec(std::make_shared<EngineObjectProperty<float>>(0.0f, "GT_DeltaSec")),
         mDeferredResourceCreators(),
         mActors(),
         mMainCamera(),
         mActiveCameras(),
         mActorControllers(),
         mMaterials(),
         mDynamicMaterials(),
         mExternalTickableObjects(),
         mTextHandler(),
#ifdef DEBUG
         mDebugUiController(std::make_unique<DebugUiController>()),
#endif
         mUiHandler(std::make_shared<UiHandler>())
   {
      LogInfo("Scene::ctor");

      RegisterEngineObject(this);
      AddEngineProperty(mGameThreadDeltaSec);
      mPhysicsWorld->InitPhysicsWorld();
   }

   void Scene::OnLevelInit()
   {
      LogInfo("Scene::OnLevelInit");
   }

   void Scene::PostLevelInit()
   {
      LogInfo("Scene::PostLevelInit");

      mTextHandler.SetScene(shared_from_this());
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

   void Scene::RegisterMainCamera(std::shared_ptr<ACamera> camera)
   {
      LogInfo("Scene::RegisterMainCamera => name = ", camera->GetCameraName());

      assert(!mMainCamera);
      mMainCamera = camera;
      RegisterCamera(camera);
   }

   void Scene::RegisterCamera(std::shared_ptr<ACamera> camera)
   {
      LogInfo("Scene::RegisterCamera => name = ", camera->GetCameraName());

      mActiveCameras.emplace_back(camera);
      RegisterEngineObject(camera.get());
      auto cameraProxyPtr = camera->CreateSceneProxy();
      camera->SceneProxyId = cameraProxyPtr->GetSceneProxyId();

      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         sceneRendererSp->CameraSceneProxyAdded_OnRenderThread(cameraProxyPtr);
      }
   }

   std::shared_ptr<MaterialProxy> Scene::RegisterMaterialInstance(std::shared_ptr<IMaterial> material)
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
      material->MaterialProxyId = materialProxy->GetSceneProxyId();

      if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
      {
         sceneRendererSp->MaterialProxyAdded_OnRenderThread(materialProxy);
      }

      return materialProxy;
   }

   InterThreadCommunicationMgr &Scene::GetInterThreadCommunicationManager()
   {
      return m_interThreadMgr;
   }

   EnginePhysics::PhysicsWorld *Scene::GetPhysicsWorld() const
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

   std::shared_ptr<Actor> Scene::GetActorById(const uint64_t id) const
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
      const std::string &goName = actor->GetEngineObjectName();
      RegisterEngineObject(actor.get());
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      const std::string &goName = actor->GetEngineObjectName();
      auto it = std::remove_if(mEngineObjects.begin(), mEngineObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetEngineObjectName() == goName; });

      if (it != mEngineObjects.end())
      {
         mEngineObjects.erase(it, mEngineObjects.end());
      }
   }

   void Scene::AddExternalTickableObject(const std::shared_ptr<ITickable> &externalTickableObject)
   {
      mExternalTickableObjects.emplace_back(externalTickableObject);
   }

   EngineObject *Scene::GetEngineObjectByName(const std::string &name) const
   {
      EngineObject *go = nullptr;

      auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [&](const auto *gameObject)
                             { return gameObject->GetEngineObjectName() == name; });

      if (it != mEngineObjects.end())
      {
         go = *it;
      }

      return go;
   }

   EngineObject *Scene::GetEngineObjectById(const uint64_t id) const
   {
      EngineObject *go = nullptr;

      auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [=](const auto *gameObject)
                             { return gameObject->GetObjectId() == id; });

      if (it != mEngineObjects.end())
      {
         go = *it;
      }

      return go;
   }

   IDeferredResourceCreator *Scene::GetDeferredResourceCreatorByName(const std::string &name) const
   {
      IDeferredResourceCreator *creatorInstance = nullptr;

      if (mDeferredResourceCreators.count(name))
      {
         creatorInstance = mDeferredResourceCreators.at(name);
      }

      return creatorInstance;
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

      actorController->InitActorController();
      mActorControllers.emplace_back(actorController);
   }

   const TextHandler &Scene::GetTextHandler() const
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

      for (auto &externalTickable : mExternalTickableObjects)
      {
         externalTickable->Tick(delta);
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

      for (auto &externalTickable : mExternalTickableObjects)
      {
         externalTickable->UnpausableTick(deltaTime);
      }

#if DEBUG
      mDebugUiController->UnpausableTick(deltaTime);
#endif

      mUiHandler->UnpausableTick(deltaTime);
   }

   void Scene::RemoveComponent(std::shared_ptr<Component> component)
   {
      const eComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
         const size_t removeProxyIndex = componentPtr->SceneProxyId;

         // delete light proxy from render thread
         if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
         {
            sceneRendererSp->PrimitiveSceneProxyDeleted_OnRenderThread(removeProxyIndex);
         }
      }
      else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<LightComponent>(component);
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

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

      RemoveEngineObject(component.get());
   }

   void Scene::RegisterComponentSceneProxy(const std::shared_ptr<Component> &component)
   {
      LogInfo("Scene::RegisterComponentSceneProxy => componentName = ", component->GetEngineObjectName());

      const eComponentType type = component->GetComponentType();
      if ((type & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
      {
         SceneComponent *sceneComponentPtr = static_cast<SceneComponent *>(component.get());
         if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent *componentPtr = static_cast<PrimitiveComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            sceneProxySp->SetDeferredShadingSceneRenderer(m_interThreadMgr.GetSceneRendererWP());
            componentPtr->SceneProxyId = sceneProxySp->GetSceneProxyId();
            sceneProxySp->SetBindedGameObjectId(componentPtr->GetObjectId());
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               sceneRendererSp->PrimitiveSceneProxyAdded_OnRenderThread(sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => primitive proxy added, sceneProxyId =", componentPtr->SceneProxyId);
         }
         else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
         {
            LightComponent *componentPtr = static_cast<LightComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->LightSceneProxyId = sceneProxySp->GetSceneProxyId();
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               sceneRendererSp->LightSceneProxyAdded_OnRenderThread(sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => ligth proxy added, sceneProxyId =", componentPtr->LightSceneProxyId);
         }
         else if ((type & eComponentType::PLANAR_REFLECTION_COMPONENT) == eComponentType::PLANAR_REFLECTION_COMPONENT)
         {
            PlanarReflectionComponent *componentPtr = static_cast<PlanarReflectionComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreatePlanarReflectionProxy();
            componentPtr->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            if (const auto &sceneRendererSp = m_interThreadMgr.GetSceneRendererWP().lock())
            {
               sceneRendererSp->BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(sceneProxySp, componentPtr->GetOwnerCamera()->SceneProxyId);
               sceneRendererSp->PlanarReflectionSceneProxyAdded_OnRenderThread(sceneProxySp);
            }
            LogInfo("Scene::RegisterComponentSceneProxy => planar reflection proxy added, sceneProxyId =", componentPtr->GetSceneProxyId());
         }
      }
   }

   std::shared_ptr<Component> Scene::CreateComponent_GameThread(const std::shared_ptr<IComponentCreatable> &componentCreator,
                                                                const ComponentData &componentData)
   {
      const auto component = componentCreator->CreateComponent(shared_from_this(), componentData);
      component->SetScene(shared_from_this());
      RegisterComponentSceneProxy(component);
      RegisterEngineObject(component.get());
      component->OnPostInitialized();
      return component;
   }

   bool Scene::RegisterDeferredResourceCreator(IDeferredResourceCreator *creatorInstance, const std::string &gameObjectName)
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
      assert(!mLuaReplicators.count(replicatorId));
      mLuaReplicators.emplace(std::make_pair(replicatorId, replicator));

      return true;
   }

   bool Scene::RemoveEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase> &replicator)
   {
      const auto replicatorId = replicator->GetReplicatorId();
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

   bool Scene::RegisterEngineObject(EngineObject *const gameObjectPtr)
   {
      const auto objectId = gameObjectPtr->GetObjectId();
      auto it = std::find_if(mEngineObjects.begin(), mEngineObjects.end(), [objectId](const auto *gameObject)
                             { return gameObject->GetObjectId() == objectId; });

      assert(it == mEngineObjects.end());

      // Add game object
      mEngineObjects.emplace_back(gameObjectPtr);

      return true;
   }

   bool Scene::RemoveEngineObject(EngineObject *const gameObjectPtr)
   {
      const std::string &goName = gameObjectPtr->GetEngineObjectName();

      auto it = std::remove_if(mEngineObjects.begin(), mEngineObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetEngineObjectName() == goName; });

      if (it != mEngineObjects.end())
      {
         mEngineObjects.erase(it, mEngineObjects.end());
         return true;
      }

      return false;
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

   Scene::~Scene()
   {
      delete mPhysicsWorld;
   }
}
