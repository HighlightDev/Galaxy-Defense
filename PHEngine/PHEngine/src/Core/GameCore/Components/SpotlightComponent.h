#pragma once

#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/RenderData/SpotlightRenderData.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace EngineCore
{
   struct LightComponentData;
   class SpotlightComponent
      : public PointLightComponent
   {
      using Base = PointLightComponent;

   public:

      SpotlightComponent(const LightComponentData& data);

      virtual ~SpotlightComponent();

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::shared_ptr<SpotlightRenderData> GetRenderData() const;
   };
}

