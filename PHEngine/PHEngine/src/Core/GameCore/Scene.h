#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentCreatorFactory.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/PlayerController.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"

using namespace Thread;

class Graphics::Proxy::LightSceneProxy;
class Graphics::Proxy::PrimitiveSceneProxy;
class EnginePhysics::PhysicsWorld;

namespace Game
{
   class Scene 
   {
   public:

      std::unordered_map<std::string, GameObject*> GameObjects;

      PlayerController m_playerController;

      class EnginePhysics::PhysicsWorld* mPhysicsWorld;

   private:

      std::vector<std::shared_ptr<Actor>> AllActors;

      InterThreadCommunicationMgr& m_interThreadMgr;

      class ICamera* m_camera;

   public:

      Scene(InterThreadCommunicationMgr& interThreadMgr);

      void PostLevelInit(std::weak_ptr<Scene> thisWeakPtr);

      void PostPhysicsInitialize();

      void AddCamera(class ICamera* camera);

      inline class ICamera* GetCamera() const
      {
         return m_camera;
      }

      inline InterThreadCommunicationMgr& GetThreadManager()
      {
         return m_interThreadMgr;
      }

      GameObject* GetGameObjectByName(const std::string& name) const;

      const std::vector<std::shared_ptr<Actor>>& GetAllActors() const;

      void AddActor(std::shared_ptr<Actor> actor);

      void RemoveActor(std::shared_ptr<Actor> actor);

      void Tick_GameThread(float delta);

      void OnUpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void OnUpdatePrimitiveComponentVisibility_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility);

      void OnUpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void RemoveComponent_GameThread(std::shared_ptr<Component> component);

      void ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& gameThreadJobCallback) const;

      void ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& renderThreadJobCallback) const;

      void PrimitiveSceneProxyDeleted(size_t primitiveSceneProxyIndex);

      void PrimitiveSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy);

      void PrimitiveSceneProxiesUpdated();

      void LightSceneProxyDeleted(size_t lightSceneProxyIndex);

      void LightSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy);

      void LightSceneProxiesUpdated();

#if DEBUG
      void UpdatePhysicsRenderData(const DebugPhysicsRenderData& physRenderData);
#endif

      ~Scene();

      template <ComponentMetaType metaType, typename ComponentT>
      std::shared_ptr<Component> CreateComponent_GameThread(const ComponentData& componentData)
      {
         auto component = ComponentCreatorFactory<metaType, ComponentT>::CreateComponent(componentData);
         ComponentType type = component->GetComponentType();
         if ((type & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
         {
            SceneComponent* sceneComponentPtr = static_cast<SceneComponent*>(component.get());
            sceneComponentPtr->SetScene(this);
            if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
            {
               PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(sceneComponentPtr);
               componentPtr->PrimitiveProxyComponentId = PrimitiveComponent::TotalPrimitiveSceneProxyIndex++;
               auto sceneProxyShared = componentPtr->CreateSceneProxy();
               PrimitiveSceneProxyAdded(componentPtr->PrimitiveProxyComponentId, sceneProxyShared);
            }
            else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
            {
               LightComponent* componentPtr = static_cast<LightComponent*>(sceneComponentPtr);
               componentPtr->LightSceneProxyId = LightComponent::TotalLightSceneProxyId++;
               auto lightProxyShared = componentPtr->CreateSceneProxy();
               LightSceneProxyAdded(componentPtr->LightSceneProxyId, lightProxyShared);
            }
         }

         const std::string& goName = component->GameObjectName;

         // Add game object
         assert(!GameObjects.count(goName));
         GameObjects[goName] = component.get();

         return component;
      }
   };

}

