#pragma once

#include "LightSceneProxy.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"

using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      class PointLightSceneProxy :
         public LightSceneProxy
      {
      protected:

         glm::vec3 m_attenuation;

         float m_radianceRadius;

      public:
         
         PointLightSceneProxy(const PointLightComponent* component);

         virtual ~PointLightSceneProxy();

         virtual void PostLevelInit() override;

         glm::vec3 GetPosition() const;

         glm::vec3 GetAttenuation() const;

         float GetRadianceRadius() const;

         virtual LightSceneProxyType GetLightProxyType() const override;

         ProjectedPointLightShadowInfo* GetProjectedPointShadowInfo();

      private:

         virtual ProjectedShadowInfo* GetShadowInfo() override;
      };

   }
}

