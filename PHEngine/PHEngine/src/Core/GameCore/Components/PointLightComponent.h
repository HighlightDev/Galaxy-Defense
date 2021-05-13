#pragma once

#include "LightComponent.h"
#include "Core/GraphicsCore/RenderData/PointLightRenderData.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;

using namespace Event;

namespace Game
{
   class PointLightComponent
      : public LightComponent
      , public PhysicsSimulationUpdatedEvent
      , public KinematicBodyMovedEvent
      , public PlayerMovedEvent
   {
      using Base = LightComponent;

   protected:

      PointLightRenderData m_renderData;

   public:

      PointLightComponent(const std::string& gameObjectName, glm::vec3 translation, const PointLightRenderData& renderData);

      virtual ~PointLightComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      inline const PointLightRenderData& GetRenderData() const {

         return m_renderData;
      }

   protected:

      virtual void ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const KinematicBodyMovedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const PlayerMovedEvent::EventData_t& data) override;

   private:

      void NotifySceneProxyThatShadowmapIsDirty(const uint64_t& functionId);
   };
}

