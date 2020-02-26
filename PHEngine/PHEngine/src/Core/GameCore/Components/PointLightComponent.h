#pragma once

#include "LightComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/RenderData/PointLightRenderData.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace Game
{
   class PointLightComponent :
      public LightComponent
   {
      using Base = LightComponent;

   protected:

      PointLightRenderData m_renderData;

   public:

      PointLightComponent(glm::vec3 translation, glm::vec3 rotation, const PointLightRenderData& renderData);

      virtual ~PointLightComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      inline const PointLightRenderData& GetRenderData() const {

         return m_renderData;
      }
   };
}

