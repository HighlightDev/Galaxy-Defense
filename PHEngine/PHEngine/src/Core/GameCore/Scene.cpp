#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"

using namespace Graphics;

namespace Game
{

   Scene::Scene(InterThreadCommunicationMgr& interThreadMgr)
      : GameObject("EngineScene")
      , mPhysicsWorld(new PhysicsWorld())
      , m_interThreadMgr(interThreadMgr)
      , mGameThreadDeltaSec(EngineGOProperty<float>(0.0f, "GT_DeltaSec"))
      , mActiveCameras()
   {
      RegisterGameObject(this);
      mPhysicsWorld->InitPhysicsWorld();
      ENGINE_PROPERTY(&mGameThreadDeltaSec);
   }

   void Scene::PostLevelInit()
   {
      for (auto& actor : mActors)
      {
         actor->SetScene(mMeSharedPtr);
         actor->PostLevelInit();
      }

      for (auto& camera : mActiveCameras)
      {
         camera->PostLevelInit();
      }
   }

   void Scene::PostPhysicsInitialize()
   {
      for (auto& actor : mActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::SetMeSharedPtr(std::shared_ptr<Scene> meSharedPtr)
   {
      mMeSharedPtr = meSharedPtr;
   }

   std::shared_ptr<Scene> Scene::GetSharedFromMe()
   {
      return shared_from_this();
   }

   void Scene::RegisterMainCamera(std::shared_ptr<ACamera> camera)
   {
      assert(!mMainCamera);
      mMainCamera = camera;
      RegisterCamera(camera);
   }

   void Scene::RegisterCamera(std::shared_ptr<ACamera> camera)
   {
      mActiveCameras.emplace_back(camera);
      RegisterGameObject(camera.get());
      auto cameraProxyPtr = camera->CreateSceneProxy();
      camera->SceneProxyId = cameraProxyPtr->GetSceneProxyId();
      CameraSceneProxyAdded_OnRenderThread(cameraProxyPtr);
   }

   std::shared_ptr<MaterialProxy> Scene::RegisterMaterialInstance(std::shared_ptr<IMaterial> material)
   {
      mMaterials.push_back(material);

      if (material->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC)
      {
         auto dynamicMaterial = std::static_pointer_cast<DynamicMaterial>(material);
         dynamicMaterial->SetScene(mMeSharedPtr);
         mDynamicMaterials.push_back(dynamicMaterial);
      }

      const auto& materialProxy = material->CreateMaterialProxy();
      material->MaterialProxyId = materialProxy->GetSceneProxyId();

      MaterialProxyAdded_OnRenderThread(materialProxy->GetSceneProxyId(), materialProxy);

      return materialProxy;
   }

   const InterThreadCommunicationMgr& Scene::GetThreadManager() const
   {
      return m_interThreadMgr;
   }

   std::shared_ptr<ACamera> Scene::GetCamera(const std::string& cameraName) const
   {
      auto cameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [&](const auto& cameraPtr) { return cameraPtr->GetGameObjectName() == cameraName; });

      if (cameraIt != mActiveCameras.end())
      {
         return (*cameraIt);
      }

      return nullptr;
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

   std::shared_ptr<IMaterial> Scene::GetMaterialByProxyId(const size_t proxyId) const
   {
      const auto materialIt = std::find_if(mMaterials.begin(), mMaterials.end(), [=](const auto& material) { return material->MaterialProxyId == proxyId; });
      return materialIt != mMaterials.end() ? *materialIt : nullptr;
   }

   void Scene::AddActor(std::shared_ptr<Actor> actor)
   {
      const std::string& goName = actor->GetGameObjectName();
      RegisterGameObject(actor.get());
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      const std::string& goName = actor->GetGameObjectName();
      if (!GameObjects.count(goName))
      {
         GameObjects.erase(goName);
      }

      const std::vector<std::shared_ptr<Actor>>::const_iterator it = std::find(mActors.begin(), mActors.end(), actor);
      if (it != mActors.end())
         mActors.erase(it);
   }

   GameObject* Scene::GetGameObjectByName(const std::string& name) const
   {
      GameObject* go = nullptr;

      if (GameObjects.count(name))
      {
         go = GameObjects.at(name);
      }

      return go;
   }

   IDeferredResourceCreator* Scene::GetDeferredResourceCreatorByName(const std::string& name) const
   {
      IDeferredResourceCreator* creatorInstance = nullptr;

      if (mDeferredResourceCreators.count(name))
      {
         creatorInstance = mDeferredResourceCreators.at(name);
      }

      return creatorInstance;
   }

   std::shared_ptr<PlayerController> Scene::GetPlayerController() const
   {
      return mPlayerController;
   }

   void Scene::SetPlayerController(std::shared_ptr<PlayerController> playerController)
   {
      mPlayerController = playerController;
   }

   void Scene::ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> gameThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceRenderThreadJob(policy, Job(creatorObjectId, functionId, gameThreadJobCallback));
   }

   void Scene::ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> renderThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceGameThreadJob(policy, Job(creatorObjectId, functionId, renderThreadJobCallback));
   }

   void Scene::UpdatePrimitiveComponentEnable_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->SceneProxiesMap.count(primitiveSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxiesMap[primitiveSceneProxyIndex]->SetEnabled(bEnabled);
            }));
         }
      }
   }

   void Scene::UpdatePrimitiveComponentVisibility_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->SceneProxiesMap.count(primitiveSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxiesMap[primitiveSceneProxyIndex]->SetVisibility(visibility);
            }));
         }
      }
   }

   void Scene::UpdatePrimitiveComponentTransform_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
      const uint64_t functionId, const glm::mat4& newRelativeMatrix, const BoundingBox& newTransformedBoundingBox)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->SceneProxiesMap.count(primitiveSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               auto& sceneProxy = sceneRenderer->SceneProxiesMap[primitiveSceneProxyIndex];
               sceneProxy->SetTransformationMatrix(newRelativeMatrix);
               sceneProxy->SetTransformedBoundingBox(newTransformedBoundingBox);
            }));
         }
      }
   }

   void Scene::UpdateCameraSceneProxyData_OnRenderThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera* camera)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->SceneViewsMap.count(sceneProxyId))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               auto sceneView = sceneRenderer->SceneViewsMap[sceneProxyId];
               sceneView->GetCameraProxy()->UpdateEyeVector(camera->GetEyeVector());
               sceneView->GetCameraProxy()->UpdateViewMatrix(camera->GetViewMatrix());
            }));
         }
      }
   }

   void Scene::UpdateLightComponentTransform_OnRenderThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->LightProxiesMap.count(lightSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->LightProxiesMap[lightSceneProxyIndex]->SetTransformationMatrix(newRelativeMatrix);
            }));
         }
      }
   }

   void Scene::PrimitiveSceneProxyDeleted_OnRenderThread(size_t primitiveSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxyDeleted_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->SceneProxiesMap.count(primitiveSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxiesMap.erase(primitiveSceneProxyIndex);
               sceneRenderer->SetProxiesAreDirty(true);
            }));
         }
      }
   }

   void Scene::PrimitiveSceneProxiesUpdated_OnRenderThread()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxiesUpdated_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetProxiesAreDirty(true);
         }));
      }
   }

   void Scene::LightSceneProxyDeleted_OnRenderThread(size_t lightSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxyDeleted_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (sceneRenderer->LightProxiesMap.count(lightSceneProxyIndex))
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->LightProxiesMap.erase(lightSceneProxyIndex);
               sceneRenderer->SetLightProxiesAreDirty(true);
            }));
         }
      }
   }

   void Scene::LightSceneProxiesUpdated_OnRenderThread()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxiesUpdated_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetLightProxiesAreDirty(true);
         }));
      }
   }

   void Scene::CameraSceneProxyAdded_OnRenderThread(std::shared_ptr<CameraSceneProxy> cameraSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::CameraSceneProxyAdded_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SceneViewsMap.emplace(cameraSceneProxy->GetSceneProxyId(), std::make_shared<SceneView>(cameraSceneProxy, sceneRenderer->SceneProxiesMap));
         }));
      }
   }

   void Scene::PrimitiveSceneProxyAdded_OnRenderThread(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxyAdded_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SceneProxiesMap[primitiveSceneProxyIndex] = primitiveSceneProxy;
            sceneRenderer->SetProxiesAreDirty(true);
         }));
      }
   }

   void Scene::LightSceneProxyAdded_OnRenderThread(size_t primitiveSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxyAdded_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->LightProxiesMap[primitiveSceneProxyIndex] = lightSceneProxy;
            sceneRenderer->SetLightProxiesAreDirty(true);
         }));
      }
   }

   void Scene::MaterialProxyAdded_OnRenderThread(size_t materialProxyIndex, std::shared_ptr<MaterialProxy> materialProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::MaterialProxyAdded_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->MaterialProxiesMap[materialProxyIndex] = materialProxy;
         }));
      }
   }

   void Scene::MaterialPropertiesUpdated_OnRenderThread(size_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>> properties)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::MaterialPropertiesUpdated_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]() mutable
         {
            sceneRenderer->MaterialProxiesMap.at(materialProxyIndex)->UpdateProperties(std::move(properties));
         }));
      }
   }

   void Scene::PlanarReflectionSceneProxyAdded_OnRenderThread(size_t planarReflectionSceneProxyId, std::shared_ptr<PlanarReflectionProxy> proxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PlanarReflectionSceneProxyAdded");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->PlanarReflectionProxiesMap[planarReflectionSceneProxyId] = proxy;
            sceneRenderer->SetPlanarReflectionProxiesAreDirty(true);
         }));
      }
   }

   void Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(std::shared_ptr<PlanarReflectionProxy> planarReflectionProxy, ACamera* cameraOwner)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto proxyId = cameraOwner->SceneProxyId;

         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            const auto& sceneViews = sceneRenderer->SceneViewsMap;
            if (sceneViews.count(proxyId))
            {
               planarReflectionProxy->SetSceneViewWeakPtr(sceneViews.at(proxyId));
            }
         }));
      }
   }

#if DEBUG
   void Scene::UpdatePhysicsRenderData(const DebugPhysicsRenderData& physRenderData)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::UpdatePhysicsRenderData");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetDebugPhysicsRenderData(physRenderData);
         }));
      }
   }
#endif

   void Scene::Tick_GameThread(float delta)
   {
      constexpr float physTickStep = 1.0f / 150.0f;

      mGameThreadDeltaSec.SetValue(delta);

      mPhysicsWorld->Tick(physTickStep);

      for (const auto& cameraPtr : mActiveCameras)
      {
         cameraPtr->Tick(delta);
      }

      mPlayerController->Tick(delta);

      for (auto& actor : mActors)
      {
         actor->Tick(delta);
      }

      for (auto& dynamicMaterial : mDynamicMaterials) {
         dynamicMaterial->Tick(delta);
      }

#if DEBUG
      UpdatePhysicsRenderData(mPhysicsWorld->GetDebugPhysicsRenderData());
#endif
   }

   void Scene::RemoveComponent(std::shared_ptr<Component> component)
   {
      const ComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
         const size_t removeProxyIndex = componentPtr->SceneProxyId;

         // delete light proxy from render thread
         PrimitiveSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }
      else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<LightComponent>(component);
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // delete light proxy from render thread
         LightSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }


      if (Actor* ownerActor = component->GetOwner())
      {
         if ((type & ComponentType::CHARACTER_MOVEMENT_COMPONENT) == ComponentType::CHARACTER_MOVEMENT_COMPONENT)
         {
            ownerActor->RemoveMovementComponent();
         }
         else if ((type & ComponentType::INPUT_COMPONENT) == ComponentType::INPUT_COMPONENT)
         {
            ownerActor->RemoveInputComponent();
         }
         else
         {
            ownerActor->RemoveComponent(component);
         }
      }

      RemoveGameObject(component.get());
   }

   void Scene::RegisterComponentSceneProxy(std::shared_ptr<Component> component)
   {
      ComponentType type = component->GetComponentType();
      if ((type & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
      {
         SceneComponent* sceneComponentPtr = static_cast<SceneComponent*>(component.get());
         sceneComponentPtr->SetScene(mMeSharedPtr);
         if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(sceneComponentPtr);

            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->SceneProxyId = sceneProxySp->GetSceneProxyId();
            PrimitiveSceneProxyAdded_OnRenderThread(componentPtr->SceneProxyId, sceneProxySp);
         }
         else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
         {
            LightComponent* componentPtr = static_cast<LightComponent*>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->LightSceneProxyId = sceneProxySp->GetSceneProxyId();
            LightSceneProxyAdded_OnRenderThread(componentPtr->LightSceneProxyId, sceneProxySp);
         }
         else if ((type & ComponentType::PLANAR_REFLECTION_COMPONENT) == ComponentType::PLANAR_REFLECTION_COMPONENT)
         {
            PlanarReflectionComponent* componentPtr = static_cast<PlanarReflectionComponent*>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreatePlanarReflectionProxy();
            BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(sceneProxySp, componentPtr->GetOwnerCamera());
            componentPtr->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            PlanarReflectionSceneProxyAdded_OnRenderThread(componentPtr->GetSceneProxyId(), sceneProxySp);
         }
      }
   }

   bool Scene::RegisterDeferredResourceCreator(IDeferredResourceCreator* creatorInstance, const std::string& gameObjectName)
   {
      // Add deferred resource creator instance
      assert(!mDeferredResourceCreators.count(gameObjectName));
      mDeferredResourceCreators[gameObjectName] = creatorInstance;

      return true;
   }

   bool Scene::RemoveDeferredResourceCreator(const std::string& gameObjectName)
   {
      // Remove deferred resource creator instance
      if (mDeferredResourceCreators.count(gameObjectName))
      {
         mDeferredResourceCreators.erase(gameObjectName);
         return true;
      }

      return false;
   }

   bool Scene::RegisterGameObject(GameObject* const gameObjectPtr)
   {
      const std::string& goName = gameObjectPtr->GetGameObjectName();

      // Add game object
      assert(!GameObjects.count(goName));
      GameObjects[goName] = gameObjectPtr;

      return true;
   }

   bool Scene::RemoveGameObject(GameObject* const gameObjectPtr)
   {
      const std::string& goName = gameObjectPtr->GetGameObjectName();

      // Remove game object
      if (GameObjects.count(goName))
      {
         GameObjects.erase(goName);
         return true;
      }

      return false;
   }

   Scene::~Scene()
   {
      delete mPhysicsWorld;
   }

}
