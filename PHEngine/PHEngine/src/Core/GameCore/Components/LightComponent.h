#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GraphicsCore/RenderData/LightRenderData.h"

#include <glm/vec3.hpp>
#include <memory>

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace Game
{
   struct LightComponentData;

   class LightComponent : 
      public SceneComponent
   {
      protected:

      using Base = SceneComponent;

      std::shared_ptr<LightRenderData> mLightRenderData;

   public:

      size_t LightSceneProxyId = 0;

      virtual ComponentType GetComponentType() const override;

      virtual void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const = 0;

      LightComponent(const LightComponentData& data);

      LightRenderData& GetLightRenderData();

      virtual ~LightComponent();
   };

}

