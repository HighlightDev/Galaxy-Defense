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
#include "Core/GameCore/ACamera.h"

#include <type_traits>

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

      class EnginePhysics::PhysicsWorld* mPhysicsWorld;

   private:

      std::vector<std::shared_ptr<Actor>> mActors;

      InterThreadCommunicationMgr& m_interThreadMgr;

      std::vector<std::shared_ptr<ACamera>> mActiveCameras;

      std::shared_ptr<PlayerController> mPlayerController;

   public:

      Scene(InterThreadCommunicationMgr& interThreadMgr);

      void PostLevelInit(std::weak_ptr<Scene> thisWeakPtr);

      void PostPhysicsInitialize();

      std::shared_ptr<ACamera> GetCamera(const std::string& name) const;

      inline InterThreadCommunicationMgr& GetThreadManager()
      {
         return m_interThreadMgr;
      }

      void RegisterCamera(std::shared_ptr<ACamera> camera);

      GameObject* GetGameObjectByName(const std::string& name) const;

      std::shared_ptr<PlayerController> GetPlayerController() const;

      const std::vector<std::shared_ptr<Actor>>& GetActors() const;

      void SetPlayerController(std::shared_ptr<PlayerController> playerController);

      void AddActor(std::shared_ptr<Actor> actor);

      void RemoveActor(std::shared_ptr<Actor> actor);

      void Tick_GameThread(float delta);

      void UpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
         const uint64_t functionId, const glm::mat4& newRelativeMatrix, const BoundingBox& newTransformedBoundingBox);

      void UpdatePrimitiveComponentVisibility_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility);

      void UpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void UpdateCameraSceneProxyData_GameThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera* camera);

      void RemoveComponent_GameThread(std::shared_ptr<Component> component);

      void ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& gameThreadJobCallback) const;

      void ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& renderThreadJobCallback) const;

      void CameraSceneProxyAdded(std::shared_ptr<CameraSceneProxy> cameraSceneProxy);

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
               
               auto sceneProxyShared = componentPtr->CreateSceneProxy();
               componentPtr->SceneProxyId = sceneProxyShared->GetSceneProxyId();
               PrimitiveSceneProxyAdded(componentPtr->SceneProxyId, sceneProxyShared);
            }
            else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
            {
               LightComponent* componentPtr = static_cast<LightComponent*>(sceneComponentPtr);
               auto lightProxyShared = componentPtr->CreateSceneProxy();
               componentPtr->LightSceneProxyId = lightProxyShared->GetSceneProxyId();
               LightSceneProxyAdded(componentPtr->LightSceneProxyId, lightProxyShared);
            }
         }

         const std::string& goName = component->GetGameObjectName();

         // Add game object
         assert(!GameObjects.count(goName));
         GameObjects[goName] = component.get();

         return component;
      }
   };

}

