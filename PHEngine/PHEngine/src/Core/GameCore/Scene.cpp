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
      , m_camera(nullptr)
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

   void Scene::AddCamera(ICamera* camera)
   {
      m_camera = camera;
      const std::string& goName = camera->GameObjectName;
      assert(GameObjects.count(goName) == 0);
      GameObjects[goName] = camera;
   }

   const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const
   {
      return mActors;
   }

   void Scene::AddActor(std::shared_ptr<Actor> actor)
   {
      const std::string& goName = actor->GameObjectName;
      assert(GameObjects.count(goName) == 0);
      GameObjects[goName] = actor.get();
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      const std::string& goName = actor->GameObjectName;
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

   void Scene::RemoveComponent_GameThread(std::shared_ptr<Component> component)
   {
      ComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
      {
         PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->PrimitiveProxyComponentId;
      
         // Remove proxy index offset
         for (auto& actor : mActors)
         {
            actor->RemoveComponentIndexOffset(removeProxyIndex);
         }

         PrimitiveComponent::TotalPrimitiveSceneProxyIndex--;

            // delete light proxy from render thread
         PrimitiveSceneProxyDeleted(removeProxyIndex);
      }
      if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
      {
         LightComponent* componentPtr = static_cast<LightComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // Remove proxy index offset
         for (auto& actor : mActors)
         {
            actor->RemoveComponentIndexOffset(removeProxyIndex);
         }

         LightComponent::TotalLightSceneProxyId--;

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

      const std::string& goName = component->GameObjectName;

      // Remove game object
      if (GameObjects.count(goName))
      {
         GameObjects.erase(goName);
      }
   }

   void Scene::ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& gameThreadJobCallback) const
   {
      ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, policy, Job(creatorObjectId, functionId, gameThreadJobCallback));
   }

   void Scene::ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& renderThreadJobCallback) const
   {
      ENQUEUE_GAME_THREAD_JOB(m_interThreadMgr, policy, Job(creatorObjectId, functionId, renderThreadJobCallback));
   }

   void Scene::OnUpdatePrimitiveComponentVisibility_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (primitiveSceneProxyIndex < sceneRenderer->SceneProxies.size())
         {
            ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies[primitiveSceneProxyIndex]->SetVisibility(visibility);
            }));
         }
      }
   }

   void Scene::OnUpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (primitiveSceneProxyIndex < sceneRenderer->SceneProxies.size())
         {
            ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies[primitiveSceneProxyIndex]->SetTransformationMatrix(newRelativeMatrix);
            }));
         }
      }
   }

   void Scene::OnUpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         if (lightSceneProxyIndex < sceneRenderer->LightProxies.size())
         {
            ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
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
         if (primitiveSceneProxyIndex < sceneRenderer->SceneProxies.size())
         {
            ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->SceneProxies.erase(sceneRenderer->SceneProxies.begin() + primitiveSceneProxyIndex);
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
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
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
         if (lightSceneProxyIndex < sceneRenderer->LightProxies.size())
         {
            ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::PUSH_ANYWAY,
               Job(creatorObjectId, functionId, [=]()
            {
               sceneRenderer->LightProxies.erase(sceneRenderer->LightProxies.begin() + lightSceneProxyIndex);
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
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SetLightProxiesAreDirty(true);
         }));
      }
   }

   void Scene::PrimitiveSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash("Scene::PrimitiveSceneProxyAdded");

      if (const auto& sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->SceneProxies.push_back(primitiveSceneProxy);
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
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::PUSH_ANYWAY,
            Job(creatorObjectId, functionId, [=]()
         {
            sceneRenderer->LightProxies.push_back(lightSceneProxy);
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
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
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

      m_camera->Tick(delta);

      m_playerController.Tick(delta);

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
      mActors.clear();
      delete m_camera;
   }

}
