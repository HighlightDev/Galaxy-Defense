#pragma once

#include <glm/vec3.hpp>

#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

using namespace Graphics::Proxy;

namespace Game
{
   
   class LightComponent : 
      public SceneComponent
   {
      using Base = SceneComponent;

   public:

      static size_t TotalLightSceneProxyId;

      size_t LightSceneProxyId = 0;

      virtual uint64_t GetComponentType() const override;

      virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix) override;

      virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const = 0;

      LightComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

      virtual ~LightComponent();
   };

}

