#pragma once

#include "LightComponent.h"
#include "Core/GraphicsCore/RenderData/DirectionalLightRenderData.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;
using namespace Event;

namespace EngineCore
{
   struct LightComponentData;

   class DirectionalLightComponent 
      : public LightComponent
      , public PlayerMovedEvent
      , public PhysicsComponentUpdatedEvent
   {

      using Base = LightComponent;

   public:

      DirectionalLightComponent(const LightComponentData& lightComponentData);

      virtual ~DirectionalLightComponent();

      virtual ComponentType GetComponentType() const override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::shared_ptr<DirectionalLightRenderData> GetRenderData() const;

      virtual void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix) override;

      virtual void Tick(const float deltaTime) override;

   protected:

      virtual void ProcessEvent(const typename PlayerMovedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const typename PhysicsComponentUpdatedEvent::EventData_t& data) override;

   private:

      void ForceUpdateShadowMap();
   };

}

