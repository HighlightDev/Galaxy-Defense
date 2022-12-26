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

      ~DirectionalLightComponent() override;

      eComponentType GetComponentType() const override;

      std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::shared_ptr<DirectionalLightRenderData> GetRenderData() const;

      void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix) override;

      void Tick(const float deltaTime) override;

   protected:

      void ProcessEvent(const typename PlayerMovedEvent::EventData_t& data) override;
      void ProcessEvent(const typename PhysicsComponentUpdatedEvent::EventData_t& data) override;

   private:

      void ForceUpdateShadowMap();
   };

}

