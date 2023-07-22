#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GraphicsCore/RenderData/LightRenderData.h"

#include <glm/vec3.hpp>
#include <memory>
#include <atomic>

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace EngineCore
{
   struct LightComponentData;

   class LightComponent : 
      public SceneComponent
   {
      protected:

      using Base = SceneComponent;

      std::shared_ptr<LightRenderData> mLightRenderData;

      std::atomic<bool> bIsSceneProxyReady{false};

      size_t mLightSceneProxyId{0};
   public:

      LightComponent(const std::shared_ptr<LightComponentData>& data);

      virtual ~LightComponent();

      void SetIsSceneProxyReady(const bool isSceneProxyReady);

      bool IsSceneProxyReady() const;

      void SetLightSceneProxyId(const size_t lightSceneProxyId);

      size_t GetLightSceneProxyId() const;

      eComponentType GetComponentType() const override;

      void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const = 0;

      LightRenderData& GetLightRenderData();

   };

}

