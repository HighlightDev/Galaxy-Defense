#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"

namespace Game
{

   Scene::Scene(InterThreadCommunicationMgr& interThreadMgr)
      : m_interThreadMgr(interThreadMgr)
      //, m_camera(new FirstPersonCamera(glm::vec3(0, 0, 1), glm::vec3(0, 0, -10)))
      , m_camera(new ThirdPersonCamera(glm::vec3(0.5f, -0.8f, 0), 20))
   {
      
   }

   void Scene::PostConstructorInitialize()
   {
      for (auto& lightProxy : LightProxies)
      {
         lightProxy->PostConstructorInitialize();
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
         std::shared_ptr<PrimitiveSceneProxy> removeProxy = SceneProxies.at(removeProxyIndex);
      
         // Remove proxy index offset
         for (auto& actor : AllActors)
         {
            actor->RemoveComponentIndexOffset(removeProxyIndex);
         }

         auto proxyIt = std::find(SceneProxies.begin(), SceneProxies.end(), removeProxy);
         if (proxyIt != SceneProxies.end())
         {
            SceneProxies.erase(proxyIt);
            PrimitiveComponent::TotalPrimitiveSceneProxyIndex--;
         }
      }
      if ((type & LIGHT_COMPONENT) == LIGHT_COMPONENT)
      {
         LightComponent* componentPtr = static_cast<LightComponent*>(component.get());
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;
         std::shared_ptr<LightSceneProxy> removeProxy = LightProxies.at(removeProxyIndex);

         // Remove proxy index offset
         for (auto& actor : AllActors)
         {
            actor->RemoveComponentIndexOffset(removeProxyIndex);
         }

         auto proxyIt = std::find(LightProxies.begin(), LightProxies.end(), removeProxy);
         if (proxyIt != LightProxies.end())
         {
            LightProxies.erase(proxyIt);
            LightComponent::TotalLightSceneProxyId--;
         }
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

   void Scene::OnUpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (primitiveSceneProxyIndex < SceneProxies.size())
      {
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
           Job(creatorObjectId, functionId, [=]()
         {
            SceneProxies[primitiveSceneProxyIndex]->SetTransformationMatrix(newRelativeMatrix);
         }));
      }
   }

   void Scene::OnUpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix)
   {
      if (lightSceneProxyIndex < LightProxies.size())
      {
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
            Job(creatorObjectId, functionId, [=]()
         {
            LightProxies[lightSceneProxyIndex]->SetTransformationMatrix(newRelativeMatrix);
         }));
      }
   }

   void Scene::Tick_GameThread(float delta)
   {
      m_camera->Tick(delta);
      for (auto& actor : AllActors)
      {
         actor->Tick(delta);
      }
      m_playerController.Tick(delta);
   }

   Scene::~Scene()
   {
      AllActors.clear();
   }

}
