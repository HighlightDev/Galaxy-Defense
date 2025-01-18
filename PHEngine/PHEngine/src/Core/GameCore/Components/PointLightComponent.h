#pragma once

#include "LightComponent.h"
#include "Core/GraphicsCore/RenderData/PointLightRenderData.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;


using namespace Event;

namespace EngineCore
{
   struct LightComponentData;

   class PointLightComponent
      : public LightComponent
      , public PhysicsComponentUpdatedGameThreadEvent
      , public KinematicBodyMovedGameThreadEvent
      , public PlayerMovedGameThreadEvent
   {
      using Base = LightComponent;

   public:

      PointLightComponent(const std::shared_ptr<LightComponentData>& lightComponentData);

      ~PointLightComponent() override;

      void Initialize() override;

      eComponentType GetComponentType() const override;

      void Tick(const float deltaTime) override;

      std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::shared_ptr<PointLightRenderData> GetRenderData() const;

   protected:

      void ProcessEvent(const PhysicsComponentUpdatedGameThreadEvent* sender, const typename PhysicsComponentUpdatedGameThreadEvent::EventData_t& data) override;
      void ProcessEvent(const KinematicBodyMovedGameThreadEvent* sender, const typename KinematicBodyMovedGameThreadEvent::EventData_t& data) override;
      void ProcessEvent(const PlayerMovedGameThreadEvent* sender, const typename PlayerMovedGameThreadEvent::EventData_t& data) override;

   private:

      void NotifySceneProxyThatShadowmapIsDirty(const uint64_t& functionId);
   };
}

