#pragma once

#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/RenderData/SpotlightRenderData.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace Game
{
   class SpotlightComponent
      : public PointLightComponent
   {
      using Base = PointLightComponent;

   protected:

     SpotlightRenderData m_renderData;

   public:

      SpotlightComponent(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation, const SpotlightRenderData& renderData);

      virtual ~SpotlightComponent();

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      inline const SpotlightRenderData& GetRenderData() const {

         return m_renderData;
      }
   };
}

