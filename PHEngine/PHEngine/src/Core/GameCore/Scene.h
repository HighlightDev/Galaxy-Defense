#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentCreatorFactory.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/PlayerController.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/Components/PrimitiveComponent.h"

using namespace Graphics::Proxy;
using namespace Thread;

namespace Game
{

   class Scene
   {
   public:

      std::vector<std::shared_ptr<Actor>> AllActors;

      std::vector<std::shared_ptr<PrimitiveSceneProxy>> SceneProxies;

      std::vector<std::shared_ptr<LightSceneProxy>> LightProxies;

      std::vector<std::shared_ptr<PrimitiveSceneProxy>> ShadowGroupPrimitives;

      PlayerController m_playerController;

   private:

      InterThreadCommunicationMgr& m_interThreadMgr;

      class ICamera* m_camera;

   public:

      Scene(InterThreadCommunicationMgr& interThreadMgr);

      void PostConstructorInitialize();

      inline class ICamera* GetCamera() const
      {
         return m_camera;
      }

      void Tick_GameThread(float delta);

      void OnUpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void OnUpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void RemoveComponent_GameThread(std::shared_ptr<Component> component);

      void ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& gameThreadJobCallback) const;

      void ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, const std::function<void(void)>& renderThreadJobCallback) const;

      ~Scene();

      template <typename PrimitiveType>
      void CreateAndAddComponent_GameThread(ComponentData& componentData, std::shared_ptr<Actor> owner)
      {
         auto component = ComponentCreatorFactory<PrimitiveType>::CreateComponent(componentData);
         uint64_t type = component->GetComponentType();
         if ((type & SCENE_COMPONENT) == SCENE_COMPONENT)
         {
            SceneComponent* sceneComponentPtr = static_cast<SceneComponent*>(component.get());
            sceneComponentPtr->SetScene(this);
            if ((type & PRIMITIVE_COMPONENT) == PRIMITIVE_COMPONENT)
            {
               PrimitiveComponent* componentPtr = static_cast<PrimitiveComponent*>(sceneComponentPtr);
               componentPtr->PrimitiveProxyComponentId = PrimitiveComponent::TotalPrimitiveSceneProxyIndex++;
               auto sceneProxyShared = componentPtr->CreateSceneProxy();
               SceneProxies.push_back(sceneProxyShared);
            }
            else if ((type & LIGHT_COMPONENT) == LIGHT_COMPONENT)
            {
               LightComponent* componentPtr = static_cast<LightComponent*>(sceneComponentPtr);
               componentPtr->LightSceneProxyId = LightComponent::TotalLightSceneProxyId++;
               auto lightProxyShared = componentPtr->CreateSceneProxy();
               LightProxies.push_back(lightProxyShared);
            }
         }

         if ((type & MOVEMENT_COMPONENT) == MOVEMENT_COMPONENT)
         {
            owner->AddMovementComponent(component);
         }
         else if ((type & INPUT_COMPONENT) == INPUT_COMPONENT)
         {
            owner->AddInputComponent(component);
         }
         else
         {
            owner->AddComponent(component);
         }
      }
   };

}

