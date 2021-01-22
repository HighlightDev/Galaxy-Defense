#pragma once

#include "LightComponent.h"
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

      PointLightComponent(const std::string& gameObjectName, glm::vec3 translation, const PointLightRenderData& renderData);

      virtual ~PointLightComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      inline const PointLightRenderData& GetRenderData() const {

         return m_renderData;
      }
   };
}

