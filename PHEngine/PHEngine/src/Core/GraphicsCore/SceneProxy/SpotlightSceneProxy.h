#pragma once

#include "PointLightSceneProxy.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"

using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      class SpotlightSceneProxy :
         public PointLightSceneProxy
      {
         float mCutoff;

      public:

         SpotlightSceneProxy(const SpotlightComponent* component);

         virtual ~SpotlightSceneProxy();

         virtual void PostLevelInit() override;

         glm::vec3 GetDirection() const;

         glm::vec3 GetTransformedDirectionVector(const glm::vec4& initialDirection) const;

         float GetCutoff() const;

         virtual LightSceneProxyType GetLightProxyType() const override;

         ProjectedSpotlightShadowInfo* GetProjectedSpotLightShadowInfo();

      private:

         virtual ProjectedShadowInfo* GetShadowInfo() override;
      };

   }
}

