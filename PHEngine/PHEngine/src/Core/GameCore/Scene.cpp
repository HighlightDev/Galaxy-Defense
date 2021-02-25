#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"

using namespace Graphics;

namespace Game
{

   Scene::Scene(InterThreadCommunicationMgr& interThreadMgr)
      : m_interThreadMgr(interThreadMgr)
      , mActiveCameras()
      , mPhysicsWorld(new PhysicsWorld())
   {
      mPhysicsWorld->InitPhysicsWorld();
   }

   void Scene::PostLevelInit(std::weak_ptr<Scene> thisWeakPtr)
   {
      for (auto& actor : mActors)
      {
         actor->PostLevelInit();
         actor->SetScene(thisWeakPtr);
      }
   }

   void Scene::PostPhysicsInitialize()
   {
      for (auto& actor : mActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::RegisterCamera(std::shared_ptr<ACamera> camera)
   {
      mActiveCameras.emplace_back(camera);
      const std::string& goName = camera->GetGameObjectName();
      assert(GameObjects.count(goName) == 0);
      GameObjects[goName] = camera.get();

      auto cameraProxyPtr = camera->CreateSceneProxy();
      camera->SceneProxyId = cameraProxyPtr->GetSceneProxyId();
      CameraSceneProxyAdded(cameraProxyPtr);
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

   const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const
   {
      return mActors;
   }

   void Scene::AddActor(std::shared_ptr<Actor> actor)
   {
      const std::string& goName = actor->GetGameObjectName();
      assert(GameObjects.count(goName) == 0);
      GameObjects[goName] = actor.get();
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

   std::shared_ptr<PlayerController> Scene::GetPlayerController() const
   {
      return mPlayerController;
   }

   void Scene::SetPlayerController(std::shared_ptr<PlayerController> playerController)
   {
      mPlayerController = playerController;
   }

   void Scene::RemoveComponent_GameThread(std::shared_ptr<Component> component)
   {
      ComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
      {
         PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->SceneProxyId;

         // delete light proxy from render thread
         PrimitiveSceneProxyDeleted(removeProxyIndex);
      }
      if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
      {
         LightComponent* componentPtr = static_cast<LightComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // delete light proxy from render thread
         LightSceneProxyDeleted(removeProxyIndex);
      }

      Actor* ownerActor = component->GetOwner();
      if (ownerActor)
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

      const std::string& goName = component->GetGameObjectName();

      // Remove game object
      if (GameObjects.count(goName))
      {
         GameObjects.erase(goName);
      }
   }

   void Scene::ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> gameThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceRenderThreadJob(policy, Job(creatorObjectId, functionId, gameThreadJobCallback));
   }

   void Scene::ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> renderThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceGameThreadJob(policy, Job(creatorObjectId, functionId, renderThreadJobCallback));
   }

   void Scene::UpdatePrimitiveComponentEnable_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->SceneProxies.count(primitiveSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies[primitiveSceneProxyIndex]->SetEnabled(bEnabled);
            }));
         }
      }
   }

   void Scene::UpdatePrimitiveComponentVisibility_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->SceneProxies.count(primitiveSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies[primitiveSceneProxyIndex]->SetVisibility(visibility);
            }));
         }
      }
   }

   void Scene::UpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
      const uint64_t functionId, const glm::mat4& newRelativeMatrix, const BoundingBox& newTransformedBoundingBox)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->SceneProxies.count(primitiveSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               auto& sceneProxy = sceneRenderer->SceneProxies[primitiveSceneProxyIndex];
               sceneProxy->SetTransformationMatrix(newRelativeMatrix);
               sceneProxy->SetTransformedBoundingBox(newTransformedBoundingBox);
            }));
         }
      }
   }

   void Scene::UpdateCameraSceneProxyData_GameThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera* camera)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->SceneViews.count(sceneProxyId));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               auto sceneView = sceneRenderer->SceneViews[sceneProxyId];
               sceneView->GetCameraProxy()->UpdateEyeVector(camera->GetEyeVector());
               sceneView->GetCameraProxy()->UpdateViewMatrix(camera->GetViewMatrix());
            }));
         }
      }
   }

   void Scene::UpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->LightProxies.count(lightSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->LightProxies[lightSceneProxyIndex]->SetTransformationMatrix(newRelativeMatrix);
            }));
         }
      }
   }

   void Scene::PrimitiveSceneProxyDeleted(size_t primitiveSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxyDeleted");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->SceneProxies.count(primitiveSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies.erase(primitiveSceneProxyIndex);
               sceneRenderer->SetProxiesAreDirty(true);
            }));
         }
      }
   }

   void Scene::PrimitiveSceneProxiesUpdated()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxiesUpdated");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetProxiesAreDirty(true);
         }));
      }
   }

   void Scene::LightSceneProxyDeleted(size_t lightSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxyDeleted");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         assert(sceneRenderer->LightProxies.count(lightSceneProxyIndex));
         {
            m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->LightProxies.erase(lightSceneProxyIndex);
               sceneRenderer->SetLightProxiesAreDirty(true);
            }));
         }
      }
   }

   void Scene::LightSceneProxiesUpdated()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxiesUpdated");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetLightProxiesAreDirty(true);
         }));
      }
   }

   void Scene::CameraSceneProxyAdded(std::shared_ptr<CameraSceneProxy> cameraSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::CameraSceneProxyAdded");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SceneViews.emplace(cameraSceneProxy->GetSceneProxyId(), std::make_shared<SceneView>(cameraSceneProxy, sceneRenderer->SceneProxies));
         }));
      }
   }

   void Scene::PrimitiveSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxyAdded");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SceneProxies[primitiveSceneProxyIndex] = primitiveSceneProxy;
            sceneRenderer->SetProxiesAreDirty(true);
         }));
      }
   }

   void Scene::LightSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::LightSceneProxyAdded");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->LightProxies[primitiveSceneProxyIndex] = lightSceneProxy;
            sceneRenderer->SetProxiesAreDirty(true);
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
      constexpr float physTickStep = 1.0f / 200.0f;

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

#if DEBUG
      UpdatePhysicsRenderData(mPhysicsWorld->GetDebugPhysicsRenderData());
#endif
   }

   Scene::~Scene()
   {
      delete mPhysicsWorld;
   }

}
