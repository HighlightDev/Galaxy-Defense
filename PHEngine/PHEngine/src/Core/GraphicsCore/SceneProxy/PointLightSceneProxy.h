#pragma once

#include "LightSceneProxy.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointShadowInfo.h"

using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      class PointLightSceneProxy :
         public LightSceneProxy
      {
         glm::vec3 m_attenuation;

         float m_radianceSqrRadius;

      public:
         
         PointLightSceneProxy(const PointLightComponent* component);

         virtual ~PointLightSceneProxy();

         virtual void PostConstructorInitialize() override;

         glm::vec3 GetPosition() const;

         glm::vec3 GetAttenuation() const;

         float GetRadianceSqrRadius() const;

         virtual LightSceneProxyType GetLightProxyType() const override;

         ProjectedPointShadowInfo* GetProjectedPointShadowInfo();

      private:

         virtual ProjectedShadowInfo* GetShadowInfo() override;
      };

   }
}

