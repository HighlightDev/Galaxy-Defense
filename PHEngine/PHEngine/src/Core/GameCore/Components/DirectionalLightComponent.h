#pragma once

#include "LightComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/RenderData/DirectionalLightRenderData.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;
using namespace Event;

namespace Game
{

   class DirectionalLightComponent 
      : public LightComponent
      , public PlayerMovedEvent
      , public SceneComponentTransformChangedEvent
   {

      using Base = LightComponent;

   protected:

      DirectionalLightRenderData m_renderData;

   public:

      DirectionalLightComponent(glm::vec3 rotation, const DirectionalLightRenderData& renderData);

      virtual ~DirectionalLightComponent();

      virtual uint64_t GetComponentType() const override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      inline const DirectionalLightRenderData& GetRenderData() const {

         return m_renderData;
      }

      virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix) override;

      virtual void Tick(const float deltaTime) override;

   protected:

      virtual void ProcessEvent(const PlayerMovedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const SceneComponentTransformChangedEvent::EventData_t& data) override;
   };

}

