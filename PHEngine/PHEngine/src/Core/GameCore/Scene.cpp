#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics;

namespace Game
{

   Scene::Scene(InterThreadCommunicationMgr& interThreadMgr)
      : m_interThreadMgr(interThreadMgr)
      // m_camera(new FirstPersonCamera(" Test camera ", glm::vec3(0, 0, 1), glm::vec3(0, 0, -10)))
      , m_camera(new ThirdPersonCamera("MainCamera", 50, 20 , 20))
      , mPhysicsWorld(new PhysicsWorld())
   {
      mPhysicsWorld->InitPhysicsWorld();
   }

   void Scene::PostConstructorInitialize()
   {
   }

   void Scene::PostPhysicsInitialize()
   {
      for (auto& actor : AllActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::RemoveComponent_GameThread(std::shared_ptr<Component> component)
   {
       uint64_t type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & PRIMITIVE_COMPONENT) == PRIMITIVE_COMPONENT)
      {
         PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->PrimitiveProxyComponentId;
      
         // Remove proxy index offset
         for (auto& actor : AllActors)
         {
            actor->RemoveComponentIndexOffset(removeProxyIndex);
         }

         PrimitiveComponent::TotalPrimitiveSceneProxyIndex--;

            // delete light proxy from render thread
         PrimitiveSceneProxyDeleted(removeProxyIndex);
      }
      if ((type & LIGHT_COMPONENT) == LIGHT_COMPONENT)
      {
         LightComponent* componentPtr = static_cast<LightComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // Remove proxy index offset
         for (auto& actor : AllActors)
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
         if ((type & MOVEMENT_COMPONENT) == MOVEMENT_COMPONENT)
         {
            ownerActor->RemoveMovementComponent();
         }
         else if ((type & INPUT_COMPONENT) == INPUT_COMPONENT)
         {
            ownerActor->RemoveInputComponent();
         }
         else
         {
            ownerActor->RemoveComponent(component);
         }
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
      const float physTickStep = 1.0f / 400.0f;

      mPhysicsWorld->Tick(physTickStep);

      m_camera->Tick(delta);

      m_playerController.Tick(delta);

      for (auto& actor : AllActors)
      {
         actor->Tick(delta);
      }

#if DEBUG
      UpdatePhysicsRenderData(mPhysicsWorld->GetDebugPhysicsRenderData());
#endif
   }

   Scene::~Scene()
   {
      AllActors.clear();
   }

}
